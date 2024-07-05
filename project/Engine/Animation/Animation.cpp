#include "Animation.h"

//Microsoft::WRL::ComPtr<ID3D12Resource> Animations::lightResource_;
//WritingStyle* Animations::lightData;

std::vector<Animation> Animations::LoadAnimationFile(const std::string& directorPath, const std::string& filename) {
	std::vector<Animation> animation{};//今回作るアニメーション
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

		animation.push_back(currentAnimationData);
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


void Animations::Initialize(const std::string& directorPath, const std::string& filename, const std::string& motionPath) {
	LoadAnimation(directorPath, motionPath);
	LoadTexture(directorPath + "/" + filename);
	skeleton = SkeletonPace::CreateSkeleton(modelData.rootNode);
	skinCluster = SkinningPace::CreateSkinCuster(Engine::GetDevice(), skeleton, modelData, Engine::GetSRV(), Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	CreateResource();
	sPipeline();
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
	skinCluster = SkinningPace::CreateSkinCuster(Engine::GetDevice(), skeleton, modelData, Engine::GetSRV(), Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
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

void Animations::BoonRecursive(Skeleton& skeleton, int32_t child) {
	Joint& root = skeleton.joints[child];
	for (uint32_t childIndex : root.children) {
		root.locaalMatrix = Math::MakeAffineMatrix(root.transform.scale, root.transform.rotate, root.transform.translate);

		boonPos.push_back({ root.skeletonSpaceMatrix.m[3][0],root.skeletonSpaceMatrix.m[3][1],root.skeletonSpaceMatrix.m[3][2] });
		boonPos2.push_back({ skeleton.joints[childIndex].skeletonSpaceMatrix.m[3][0],  skeleton.joints[childIndex].skeletonSpaceMatrix.m[3][1],  skeleton.joints[childIndex].skeletonSpaceMatrix.m[3][2] });


		BoonRecursive(skeleton, childIndex);
	}
}

void Animations::SkeletonUpdate() {
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



	
	if (flameTimer_ == 0.0f) { animationTime += 1.0f / 60.0f; }
	else { animationTime += 1.0f / flameTimer_; }

	if (isLoop_) {
		animationTime = std::fmod(animationTime, animation[animationNumber].duration);//最後まで行ったら最初に戻る。リピート再生 
		localMatrix = Math::MakeIdentity4x4();
	}
	
	
}

void Animations::ComputeParameter() {
	Engine::GetList()->SetComputeRootSignature(CSRootSignature_.Get());
	Engine::GetList()->SetPipelineState(CSPipelineState_.Get());
	Engine::GetList()->SetComputeRootDescriptorTable(0,skinCluster.paletteSrvHandle.second);
	Engine::GetList()->SetComputeRootDescriptorTable(1,inputVertex);
	Engine::GetList()->SetComputeRootDescriptorTable(2,influence);
	Engine::GetList()->SetComputeRootDescriptorTable(3,outputVertex);
	Engine::GetList()->SetComputeRootConstantBufferView(4, skinningInformation->GetGPUVirtualAddress());
	Engine::GetList()->Dispatch(UINT(modelData.meshData.vertices.size() + 1023) / 1024, 1, 1);


}

void Animations::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool flag) {
	ApplyAnimation(modelData.rootNode.name, animationNumber_);
	SkeletonUpdate();
	SkinningUpdate();
	//ComputeParameter();

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
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
		meshData_->GetVertexBufferView(),
		skinCluster.influenceBufferView
	};
	Engine::GetList()->IASetVertexBuffers(0, 2, vbvs);
	Engine::GetList()->IASetIndexBuffer(&meshData_->GetIndexBufferView());


	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texture_));
	Engine::GetList()->SetGraphicsRootDescriptorTable(7, textureManager_->GetGPUHandle(maskTexture_));
	if (lightData->environment_.isEnble_) {
		Engine::GetList()->SetGraphicsRootDescriptorTable(6, textureManager_->GetGPUHandle(Skybox::textureNum));
	}
	if (modelData.rootNode.children.size() != 0) {
		Engine::GetList()->SetGraphicsRootDescriptorTable(8, skinCluster.paletteSrvHandle.second);
	}
	

	// wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialData_->GetResource()->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(5, cameraResorce_->GetGPUVirtualAddress());
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
	meshData_->Initialize(modelData.meshData);
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
	//lightData->environment_.isEnble_ = false;
	lightData->dissolve_.threshold = 0.0f;
	lightData->dissolve_.edgeColor = { 1.0f,0.4f,0.3f };
	lightData->dissolve_.isEnble = true;

	//カメラ
	cameraResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(CameraForGPU));
	cameraResorce_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	/*/UAVResourceの作成
	uavResource_ = Mesh::CreateUAVBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) *meshData_->GetVerticesSize());
	//uavResource_->Map(0, nullptr, reinterpret_cast<void**>(&uavData_));

	//スキニング
	skinningInformation = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(SkinningInformation));
	skinningInformation->Map(0, nullptr, reinterpret_cast<void**>(&skinningData_));
	skinningData_->numVertices = uint32_t(modelData.meshData.vertices.size());

	//SRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC inputDesc{};
	inputDesc.Format = DXGI_FORMAT_UNKNOWN;
	inputDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	inputDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	inputDesc.Buffer.FirstElement = 0;
	inputDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	inputDesc.Buffer.NumElements = UINT(meshData_->GetVerticesSize());
	inputDesc.Buffer.StructureByteStride = sizeof(VertexData);

	inputVertex = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Engine::GetDevice()->CreateShaderResourceView(meshData_->GetVertexResource().Get(), &inputDesc, inputVertex);

	//SRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC influenceDesc{};
	influenceDesc.Format = DXGI_FORMAT_UNKNOWN;
	influenceDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	influenceDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	influenceDesc.Buffer.FirstElement = 0;
	influenceDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	influenceDesc.Buffer.NumElements = UINT(skinCluster.mappedInfluence.size());
	influenceDesc.Buffer.StructureByteStride = sizeof(VertexInfluence);

	influence = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Engine::GetDevice()->CreateShaderResourceView(meshData_->GetVertexResource().Get(), &influenceDesc, influence);

	//srv作成
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = UINT(meshData_->GetVerticesSize());//頂点数
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = sizeof(VertexData);

	outputVertex = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// UAVビューを作成
	Engine::GetDevice()->CreateUnorderedAccessView(uavResource_.Get(), nullptr, &uavDesc, outputVertex);
	*/
	//inputVertex = Mesh::CreateResourceView(meshData_->GetVertexResource().Get(),meshData_->GetVerticesSize(),sizeof(VertexData));
	//influence = Mesh::CreateResourceView(skinCluster.influenceResource.Get(), skinCluster.mappedInfluence.size(), sizeof(VertexInfluence));
	//outputVertex = Mesh::CreateUAVView(uavResource_.Get(), meshData_->GetVerticesSize(), sizeof(VertexData));
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

