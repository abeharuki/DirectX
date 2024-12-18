#include "Animation.h"

//Microsoft::WRL::ComPtr<ID3D12Resource> Animations::lightResource_;
//WritingStyle* Animations::lightData;

std::vector<Animation> Animations::LoadAnimationFile(const std::string& directorPath, const std::string& filename) {
	std::vector<Animation> animations{};//今回作るアニメーション
	Assimp::Importer importer;
	std::string filePath = directorPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);//アニメーションがない

	for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex) {
		Animation currentAnimationData{};
		aiAnimation* animationAssimp = scene->mAnimations[animationIndex];//最初のアニメーションだけ採用
		currentAnimationData.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を1秒に変換

		//assimpでは個々のNodeのAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報を取ってくる
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			NodeAnimation& nodeAnimation = currentAnimationData.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
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

		animations.push_back(currentAnimationData);
	}

	

	return animations;
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

void Animations::Initialize(const std::string& directorPath, const std::string& filename, const std::string& motionPath) {
	LoadAnimation(directorPath, motionPath);
	LoadTexture(directorPath + "/" + filename);
	skeleton = SkeletonPace::CreateSkeleton(modelData.rootNode);
	skinCluster = SkinningPace::CreateSkinCuster( skeleton, modelData);
	CreateResource();
	sPipeline();
	CreateJointWorldTransforms();
	jointsNum_ = int(skeleton.joints.size());
	line_.resize(jointsNum_);
	localMatrix = Math::MakeIdentity4x4();
	for (int i = 0; i < jointsNum_; i++) {
		line_[i].reset(Line::CreateLine({ 0,0,0 }, { 0,0,0 }));
	}

}

void Animations::Initialize(const std::string& directorPath,const std::string& motionPath) {
	LoadAnimation(directorPath, motionPath);
	LoadTexture(modelData.material.textureFilePath);
	skeleton = SkeletonPace::CreateSkeleton(modelData.rootNode);
	skinCluster = SkinningPace::CreateSkinCuster( skeleton, modelData);
	CreateResource();
	LoadTexture(modelData.material.textureFilePath);
	sPipeline();
	jointsNum_ = int(skeleton.joints.size());
	line_.resize(jointsNum_);
	localMatrix = Math::MakeIdentity4x4();
	for (int i = 0; i < jointsNum_; i++) {
		line_[i].reset(Line::CreateLine({ 0,0,0 }, { 0,0,0 }));
	}
}


void Animations::LoadAnimation(const std::string& directorPath, const std::string& filename) {
	modelData = modelManager_->LoadGltfFile(directorPath ,filename);
	animation = LoadAnimationFile(directorPath, filename);
}

void Animations::LoadTexture(const std::string& filename) {
	textureManager_ = TextureManager::GetInstance();

	//テクスチャを設定
	if (filename != "")
	{
		TextureManager::GetInstance()->Load(filename);
		texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(filename);
	}
	else
	{
		texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/white.png");
	}

	maskTexture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/Mask/noise0.png");
	
}

void Animations::ApplyAnimation(const std::string& name,const uint32_t animationNumber) {
	if (auto it = animation[animationNumber].nodeAnimations.find(name); it != animation[animationNumber].nodeAnimations.end())
	{
		NodeAnimation& rootNodeAnimation = (*it).second;
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		localMatrix = Math::MakeAffineMatrix(scale, rotate, translate);
	}

	for (Joint& joint : skeleton.joints) {
		//対象のJointのAnimationがあれば、値の提供を行う.
		if (auto it = animation[animationNumber].nodeAnimations.find(joint.name); it != animation[animationNumber].nodeAnimations.end()) {

			if (isBlending_) {

				Vector3 currentTranslate = CalculateValue(animation[preAnimationNumber_].nodeAnimations[joint.name].translate.keyframes, animationTime);
				Quaternion currentRotate = CalculateValue(animation[preAnimationNumber_].nodeAnimations[joint.name].rotate.keyframes, animationTime);
				Vector3 currentScale = CalculateValue(animation[preAnimationNumber_].nodeAnimations[joint.name].scale.keyframes, animationTime);


				// 次のアニメーションのキーを取得
				Vector3 nextTranslate = CalculateValue(animation[animationNumber].nodeAnimations[joint.name].translate.keyframes, nextAnimationTime);
				Quaternion nextRotate = CalculateValue(animation[animationNumber].nodeAnimations[joint.name].rotate.keyframes, nextAnimationTime);
				Vector3 nextScale = CalculateValue(animation[animationNumber].nodeAnimations[joint.name].scale.keyframes, nextAnimationTime);

				// 線形補間（Lerp）および球面線形補間（Slerp）を使用してブレンド
				float t = blendTime_ / blendDuration_;
				joint.transform.translate = Math::Lerp(currentTranslate, nextTranslate, t);
				joint.transform.rotate = Math::Slerp(currentRotate, nextRotate, t);
				joint.transform.scale = Math::Lerp(currentScale, nextScale, t);

			}
			else {

				const NodeAnimation& rootNodeAnimation = (*it).second;
				joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
				joint.transform.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
				joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);

			}


		}
		
	}

	
}

void Animations::BoonRecursive(Skeleton& skeletons, int32_t child) {
	Joint& root = skeletons.joints[child];
	for (uint32_t childIndex : root.children) {
		root.locaalMatrix = Math::MakeAffineMatrix(root.transform.scale, root.transform.rotate, root.transform.translate);

		boonPos.push_back({ root.skeletonSpaceMatrix.m[3][0],root.skeletonSpaceMatrix.m[3][1],root.skeletonSpaceMatrix.m[3][2] });
		boonPos2.push_back({ skeletons.joints[childIndex].skeletonSpaceMatrix.m[3][0],  skeletons.joints[childIndex].skeletonSpaceMatrix.m[3][1],  skeletons.joints[childIndex].skeletonSpaceMatrix.m[3][2] });


		BoonRecursive(skeletons, childIndex);
	}
}

void Animations::SkeletonUpdate(const WorldTransform& worldTransform) {
	//全てのJointを更新
	for (Joint& joint : skeleton.joints) {
		joint.locaalMatrix = Math::MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.locaalMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else {
			joint.skeletonSpaceMatrix = joint.locaalMatrix;
		}




		//Scale成分を取り除く
		Matrix4x4 jointWorldTransform = joint.skeletonSpaceMatrix;
		for (int i = 0; i < 3; ++i)
		{
			float scale = std::sqrt(jointWorldTransform.m[i][0] * jointWorldTransform.m[i][0] +
				jointWorldTransform.m[i][1] * jointWorldTransform.m[i][1] +
				jointWorldTransform.m[i][2] * jointWorldTransform.m[i][2]);
			for (int j = 0; j < 3; ++j)
			{
				jointWorldTransform.m[i][j] /= scale;
			}
		}
		//ワールド行列を掛ける
		jointWorldTransforms_[joint.index].matWorld_ = jointWorldTransform *worldTransform.matWorld_;

	}

	

	//デバック表示用処理
	BoonRecursive(skeleton, skeleton.root);

	for (size_t i = 3; i < boonPos.size() && i < boonPos2.size() && i < line_.size(); ++i) {
		line_[i]->SetLinePos(boonPos[i], boonPos2[i]);
	}

	boonPos.clear();
	boonPos2.clear();

}

void Animations::SkinningUpdate() {
	//全てのJointを更新
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < skinCluster.inverseBindPoseMatrixs.size());
		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix = skinCluster.inverseBindPoseMatrixs[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Math::Transpose(Math::Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}


void Animations::Update(const uint32_t animationNumber) {

	animationNumber_ = animationNumber;

	if (preAnimationNumber_ != animationNumber) {
		isBlending_ = true;
	}

	if (isBlending_) {
		blendTime_ += 1.0f / 60.0f; // フレーム時間の更新
		if (blendTime_ >= blendDuration_) {
			// ブレンド完了
			isBlending_ = false;
			preAnimationNumber_ = animationNumber;
			animationTime = nextAnimationTime;
			blendTime_ = 0.0f;
			nextAnimationTime = 0.0f;
		}
		else {
			nextAnimationTime += 1.0f / 60.0f;
		}
	}



	if (!stop_) {
		if (flameTimer_ == 0.0f) { animationTime += 1.0f / 60.0f; }
		else { animationTime += 1.0f / flameTimer_; }

		if (isLoop_) {
			animationTime = std::fmod(animationTime, animation[animationNumber].duration);//最後まで行ったら最初に戻る。リピート再生 
			localMatrix = Math::MakeIdentity4x4();
		}
	}


	
	
	
}

void Animations::ComputeParameter() {
	if (!modelData.skinClusterData.empty()) {
		//バリアの設定
		Engine::GetInstance()->TransitionResource(*meshData_->GetOutputVertex(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		Engine::GetList()->SetComputeRootSignature(CSRootSignature_.Get());
		Engine::GetList()->SetPipelineState(CSPipelineState_.Get());
		Engine::GetList()->SetComputeRootDescriptorTable(0, skinCluster.paletteResource.get()->GetSRVHandle());
		Engine::GetList()->SetComputeRootDescriptorTable(1, meshData_->GetInputVertex()->GetSRVHandle());
		Engine::GetList()->SetComputeRootDescriptorTable(2, skinCluster.influenceResource.get()->GetSRVHandle());
		Engine::GetList()->SetComputeRootDescriptorTable(3, meshData_->GetOutputVertex()->GetUAVHandle());
		Engine::GetList()->SetComputeRootConstantBufferView(4, skinningInformation->GetGPUVirtualAddress());
		Engine::GetList()->Dispatch(UINT(modelData.meshData.vertices.size() + 1023) / 1024, 1, 1);
		//バリアの変更
		Engine::GetInstance()->TransitionResource(*meshData_->GetOutputVertex(), D3D12_RESOURCE_STATE_GENERIC_READ);
	}
}

void Animations::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool flag) {
	ApplyAnimation(modelData.rootNode.name, animationNumber_);
	SkeletonUpdate(worldTransform);
	SkinningUpdate();
	ComputeParameter();

	if (modelData.skinClusterData.empty()) {
		worldTransform.matWorld_ = localMatrix * worldTransform.matWorld_;
		worldTransform.TransferMatrix();
	}
	
	//カメラpos
	cameraData->worldPos = viewProjection.worldPos_;
	materialData_->SetLighting(flag);

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &meshData_.get()->GetVertexBufferView());
	Engine::GetList()->IASetIndexBuffer(&meshData_->GetIndexBufferView());


	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texture_));
	Engine::GetList()->SetGraphicsRootDescriptorTable(7, textureManager_->GetGPUHandle(maskTexture_));
	if (lightData->environment_.isEnble_) {
		Engine::GetList()->SetGraphicsRootDescriptorTable(6, textureManager_->GetGPUHandle(Skybox::textureNum));
	}
	
	

	// wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialData_->GetResource()->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(5, cameraResource_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());

	// 三角形の描画
	if (!debug_) {
		Engine::GetList()->DrawIndexedInstanced(UINT(meshData_->GetIndicesSize()), 1, 0, 0, 0);
	}

#ifdef _DEBUG
	for (int i = 0; i < line_.size(); i++) {
		line_[i]->Draw(worldTransform, viewProjection, false);
}
#endif

	
}


Animations* Animations::Create(const std::string& directorPath, const std::string& filename, const std::string& motionPath)
{
	Animations* anime = new Animations;
	anime->Initialize(directorPath, filename, motionPath);
	return anime;
}

Animations* Animations::Create(const std::string& directorPath,const std::string& motionPath)
{
	Animations* anime = new Animations;
	anime->Initialize(directorPath,motionPath);
	return anime;
}


void Animations::sPipeline() {

	if (modelData.rootNode.children.size() != 0) {

		//スキニングアニメーションなら
		vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateAnimationVSShader();
		pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePSShader();

		rootSignature_ = GraphicsPipeline::GetInstance()->CreateAnimationRootSignature();
		sPipelineState_ = GraphicsPipeline::GetInstance()->CreateAnimationGraphicsPipeline(blendMode_);

		GraphicsPipeline::GetInstance()->CreateAnimationCSShader();
		CSRootSignature_ = GraphicsPipeline::GetInstance()->CreateAnimationCSRootSignature();
		CSPipelineState_ = GraphicsPipeline::GetInstance()->CreateAnimationCSGraphicsPipeline();

	}
	else {
		//ビジットアニメーションなら
		vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateVSShader();
		pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePSShader();

		rootSignature_ = GraphicsPipeline::GetInstance()->CreateRootSignature();
		sPipelineState_ = GraphicsPipeline::GetInstance()->CreateGraphicsPipeline(blendMode_);
	}

};

//頂点データの設定
void Animations::CreateResource() {
	// モデルの読み込み
	meshData_ = std::make_unique<Mesh>();
	meshData_->Initialize(modelData.meshData, !modelData.skinClusterData.empty());
	//マテリアル
	materialData_ = std::make_unique<Material>();
	materialData_->Initialize();

	// ライティング
	lightResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(WritingStyle));
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

	// デフォルト値
	lightData->directionLight_.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	lightData->directionLight_.direction = { 0.0f, -1.0f, 0.0f };
	lightData->directionLight_.intensity = 1.0f;
	lightData->environment_.isEnble_ = false;
	lightData->dissolve_.threshold = 0.0f;
	lightData->dissolve_.edgeColor = { 1.0f,0.4f,0.3f };
	lightData->dissolve_.isEnble = true;

	//カメラ
	cameraResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(CameraForGPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	
	//スキニング
	skinningInformation = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(uint32_t));
	skinningInformation->Map(0, nullptr, reinterpret_cast<void**>(&skinningData));
	skinningData->numVertices = uint32_t(modelData.meshData.vertices.size());
	skinningInformation->Unmap(0, nullptr);

	

	


};


void Animations::DirectionalLightDraw(DirectionLight directionLight) {
	lightData->directionLight_.color = directionLight.color;
	lightData->directionLight_.direction = Math::Normalize(directionLight.direction);
	lightData->directionLight_.intensity = directionLight.intensity;
	lightData->directionLight_.isEnable_ = true;
}
void Animations::PointLightDraw(PointLight pointLight, Vector3 direction) {
	lightData->pointLight_ = pointLight;
	lightData->directionLight_.direction = Math::Normalize(direction);
	lightData->pointLight_.isEnable_ = true;
	lightData->directionLight_.intensity = 0.0f;

}
void Animations::SpotLightDraw(SpotLight spotLight) {
	lightData->spotLight_ = spotLight;
	lightData->spotLight_.direction_ = Math::Normalize(spotLight.direction_);
	lightData->spotLight_.isEnable_ = true;
}


void Animations::SetAnimationTimer(float startTimer, float flameTimer) {
	nextAnimationTime = startTimer;
	flameTimer_ = flameTimer;
}

void Animations::SetBlend(const uint32_t animationNumber ,float num) {
	animationNumber_ = animationNumber;
	blendDuration_ = num;
	isBlending_ = true;
	nextAnimationTime = 0.0f;
}

void Animations::SetTexture(const std::string& path) {
	TextureManager::GetInstance()->Load(path);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(path);
}

void Animations::SetMaskTexture(const std::string& path) {
	TextureManager::GetInstance()->Load("resources/Mask/" + path);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/Mask/" + path);
}

void Animations::AnimationDebug() {
	ImGui::Begin("Animations");
	ImGui::Checkbox("Debug", &debug_);
	ImGui::Text("animationTime%f",animationTime);
	ImGui::Text("nextAnimationTime%f", nextAnimationTime);
	ImGui::End();
}

void Animations::CreateJointWorldTransforms()
{
	//JointのWorldTransformを作成
	jointWorldTransforms_.resize(skeleton.joints.size());
}


const WorldTransform& Animations::GetJointWorldTransform(const std::string& name) const
{
	for (const Joint& joint : skeleton.joints)
	{
		if (joint.name == name)
		{
			return jointWorldTransforms_[joint.index];
		}
	}
	static WorldTransform emptyWorldTransform{};
	return emptyWorldTransform;
}
