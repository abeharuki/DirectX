#include "Animation.h"


Animation LoadAnimationFile(const std::string& directorPath, const std::string& filename) {
	Animation animation;//今回作るアニメーション
	Assimp::Importer importer;
	std::string filePath = directorPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);//アニメーションがない
	aiAnimation* animationAssimp = scene->mAnimations[0];//最初のアニメーションだけ採用
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を1秒に変換

	//assimpでは個々のNodeのAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報を取ってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		//位置
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//秒に変換
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}

		//回転
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe{};
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//秒に変換
			keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };//右手->左手
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}

		//スケール
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe{};
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//秒に変換
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}



	}



	return animation;
}

Vector3 Animations::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Math::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}

Quaternion Animations::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
	assert(!keyframes.empty());
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Math::Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}

Skeleton Animations::CreateSkeleton(const Node& rootNode) {
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	//名前とIndexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}
	return skeleton;
}

int32_t Animations::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint;
	joint.name = node.name;
	joint.locaalMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = Math::MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());//現在登録されている数をIndexに
	joint.parent = parent;
	joints.push_back(joint);//SkeletonのJoint列に追加
	for (const Node& child : node.children) {
		//子Jointを作成、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	return joint.index;
}

void Animations::Initialize(const std::string& directorPath, const std::string& filename, const std::string& motionPath) {

	LoadAnimation(directorPath, motionPath);
	LoadTexture(directorPath + "/" + filename);
	skeleton = CreateSkeleton(modelData.rootNode);
	CreateVertexResource();
	sPipeline();


}

void Animations::LoadAnimation(const std::string& directorPath, const std::string& filename) {
	modelData = modelManager_->LoadObjFile(directorPath + "/" + filename);
	animation = LoadAnimationFile(directorPath, filename);
}

void Animations::LoadTexture(const std::string& filename) {
	textureManager_ = TextureManager::GetInstance();
	TextureManager::GetInstance()->Load(filename);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(filename);
}

void Animations::ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime) {
	for (Joint& joint : skeleton.joints) {
		//対象のJointのAnimationがあれば、値の提供を行う.
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}

void Animations::SkeletonUpdate(Skeleton& skeleton) {
	//全てのJointを更新
	for (Joint& joint : skeleton.joints) {
		joint.locaalMatrix = Math::MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.locaalMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else {
			joint.skeletonSpaceMatrix = joint.locaalMatrix;
		}
	}
}

void Animations::SetAnimationTimer(float startTimer, float flameTimer) {
	animationTime = startTimer;
	flameTimer_ = flameTimer;
}

void Animations::Update(WorldTransform& worldTransform,bool roop) {

	if (flameTimer_ == 0.0f) {animationTime += 1.0f / 60.0f;}
	else {animationTime += 1.0f / flameTimer_;}
	if (roop) {
		animationTime = std::fmod(animationTime, animation.duration);//最後まで行ったら最初に戻る。リピート再生 
	}
	NodeAnimation& rootNodeAnimation = animation.nodeAnimations[modelData.rootNode.name];
	Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
	Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
	Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
	Matrix4x4 localMatrix = Math::MakeAffineMatrix(scale, rotate, translate);
	//worldTransform.UpdateMatrix();
	worldTransform.matWorld_ = localMatrix * worldTransform.matWorld_;
	worldTransform.TransferMatrix();

}

void Animations::Update() {
	animationTime += 1.0f / 60.0f;
	animationTime = std::fmod(animationTime, animation.duration);//最後まで行ったら最初に戻る。リピート再生
	ApplyAnimation(skeleton, animation, animationTime);
	SkeletonUpdate(skeleton);
}

void Animations::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection) {

	/*/RootのMatrixを適用
	worldTransform.matWorld_ = modelData.rootNode.localMatrix * worldTransform.matWorld_;
	worldTransform.TransferMatrix();
	*/
	//カメラpos
	cameraData->worldPos = viewProjection.worldPos_;



	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateGraphicsPipeline(blendMode_);


	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);



	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texture_));

	// wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(5, cameraResorce_->GetGPUVirtualAddress());


	// 三角形の描画
	Engine::GetList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);


}

Animations* Animations::Create(const std::string& directorPath, const std::string& filename, const std::string& motionPath)
{
	Animations* anime = new Animations;
	anime->Initialize(directorPath, filename, motionPath);
	return anime;


}


void Animations::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePSShader();


	rootSignature_ = GraphicsPipeline::GetInstance()->CreateRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateGraphicsPipeline(blendMode_);
};

//頂点データの設定
void Animations::CreateVertexResource() {
	// モデルの読み込み 
	// 頂点リソースを作る
	vertexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) * modelData.vertices.size());
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得

	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(
		sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ


	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースにコピース




	/*/ 頂点リソースを作る
	uint32_t* mappedIndex = nullptr;
	for (const Joint& joint : skeleton.joints) {
		mappedIndex = skeleton.jointMap.;
	}
	for (uint32_t i = 0; i < skeleton.jointMap.max_size(), i++;) {
		mappedIndex[i] = skeleton.jointMap;
	}
	indexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(uint32_t) * modelData.indices.size());
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex)); // 書き込むためのアドレスを取得
	//mappedIndex[]


	// 頂点バッファビューを作成する
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	indexBufferView.SizeInBytes = sizeof(uint32_t) * modelData.indices.size(); // 使用するリソースのサイズは頂点サイズ
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // 1頂点あたりのサイズ


	std::memcpy(mappedIndex, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size()); // 頂点データをリ
	/
	*/

	// マテリアル
	materialResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(Material));

	// マテリアルにデータを書き込む
	// 書き込むためのアドレスを取得
	materialResorce_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 今回は白を書き込む
	materialData->color.rgb = Vector3(1.0f, 1.0f, 1.0f);
	materialData->color.a = float(1.0f);
	// Lightingを有効にする
	materialData->enableLighting = false;
	//光沢
	materialData->shininess = 10.0f;
	// 初期化
	materialData->uvTransform = Math::MakeIdentity4x4();




	//カメラ
	cameraResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(CameraForGPU));
	cameraResorce_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

};




