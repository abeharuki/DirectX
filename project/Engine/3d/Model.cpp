#include "Model.h"
#include <cassert>
#include <format>
#include <imgui.h>



//Microsoft::WRL::ComPtr<ID3D12Resource> Model::publicLightResource_;
//WritingStyle* Model::publicLightData;

void Model::Initialize(const std::string& filename, const std::string& texturePath) { 
	modelData = ModelManager::LoadObjFile(filename);
	LoadTexture(texturePath);
	if (texturePath != "") {
		
	}
	else {
		//LoadTexture(modelData.material.textureFilePath);
	}
	
	CreateVertexResource();
	sPipeline();
	
	
}

void Model::Initialize(const std::string& filename) {
	textureManager_ = TextureManager::GetInstance();
	modelData = ModelManager::LoadObjFile("resources/JsonFile/" + filename + ".obj");
	if (modelData.material.textureFilePath != "") {
		LoadTexture("resources/JsonFile/" + modelData.material.textureFilePath);
	}
	
	CreateVertexResource();
	sPipeline();


}

void Model::NoDepthInitialize(const std::string& filename, const std::string& texturePath){
	modelData = ModelManager::LoadObjFile(filename);
	LoadTexture(texturePath);
	CreateVertexResource();
	sNoDepthPipeline();
}


void Model::sPipeline() {
	
	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePSShader();
	
	
	rootSignature_ = GraphicsPipeline::GetInstance()->CreateRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateGraphicsPipeline(blendMode_);

}
void Model::sNoDepthPipeline(){
	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePSShader();


	rootSignature_ = GraphicsPipeline::GetInstance()->CreateRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateNoDepthGraphicsPipeline(blendMode_);
};


void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection ,bool light) {
	
	//カメラpos
	cameraData->worldPos = viewProjection.worldPos_;

	//ライティング有効化
	materialData_->SetLighting(light);
	

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &meshData_->GetVertexBufferView());
	
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(texture_));
	Engine::GetList()->SetGraphicsRootDescriptorTable(7, TextureManager::GetInstance()->GetGPUHandle(maskTexture_));

	if (lightData->environment_.isEnble_) {
		Engine::GetList()->SetGraphicsRootDescriptorTable(6, textureManager_->GetGPUHandle(Skybox::textureNum));
	}

    // wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialData_->GetResource()->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(5, cameraResorce_->GetGPUVirtualAddress());

	Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());
	//Engine::GetList()->SetGraphicsRootConstantBufferView(8, dissolveResource_->GetGPUVirtualAddress());


	// 三角形の描画
	Engine::GetList()->DrawInstanced(UINT(meshData_->GetVerticesSize()), 1, 0, 0);

	
}

//頂点データの設定
void Model::CreateVertexResource() {
	// モデルの読み込み 
	meshData_ = std::make_unique<Mesh>();
	meshData_->Initialize(modelData);
	

	materialData_ = std::make_unique<Material>();
	materialData_->Initialize();

	//ディゾルブ
	//dissolveResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(DissolveStyle));
	//dissolveResource_->Map(0, nullptr, reinterpret_cast<void**>(&dissolveData));
	//dissolveData->isEnble = false;
	//dissolveData->threshold = 1.0f;
	//dissolveData->edgeColor = { 1.0f,0.4f,0.3f };

	
	// ライティング
	lightResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(WritingStyle));
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

	// デフォルト値
	lightData->directionLight_.color = {1.0f, 1.0f, 1.0f, 1.0f};
	lightData->directionLight_.direction = Math::Normalize({0.0f, -5.0f, 0.0f});
	lightData->directionLight_.intensity = 1.0f;
	lightData->directionLight_.isEnable_ = true;
	lightData->dissolve_.threshold = 0.0f;
	lightData->dissolve_.edgeColor = { 1.0f,0.4f,0.3f };
	lightData->dissolve_.isEnble = false;

	//カメラ
	cameraResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(CameraForGPU));
	cameraResorce_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

};

void Model::SetColor(Vector4 color) {
	materialData_->SetColor(color);
}

void Model::SetBlendMode(BlendMode blendMode) { 

	if (blendMode_ != blendMode) {
		blendMode_ = blendMode;
		sPipeline();
	}

	
}

void Model::SetShininess(float i) { materialData_->SetShininess(i); }

Model* Model::CreateModelFromObj(const std::string& filename, const std::string& texturePath) {
	Model* model = new Model;
	model->Initialize(filename, texturePath);
	return model;
}

Model* Model::CreateFromObj(const std::string& filename) {
	Model* model = new Model;
	model->Initialize(filename);
	return model;
}

Model* Model::CreateFromNoDepthObj(const std::string& filename, const std::string& texturePath)
{
	Model* model = new Model;
	model->NoDepthInitialize(filename, texturePath);
	return model;
}

void Model::LoadTexture(const std::string& texturePath) {
	
	textureManager_ = TextureManager::GetInstance();

	//テクスチャを設定
	if (texturePath != "")
	{
		TextureManager::GetInstance()->Load(texturePath);
		texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(texturePath);
	}
	else
	{
		texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/white.png");
	}
	maskTexture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/Mask/noise0.png");

}

void Model::DirectionalLightDraw(DirectionLight directionLight) {
	lightData->directionLight_.color = directionLight.color;
	lightData->directionLight_.direction = Math::Normalize(directionLight.direction);
	lightData->directionLight_.intensity = directionLight.intensity;
	lightData->directionLight_.isEnable_ = directionLight.isEnable_;
	
}

void Model::PointLightDraw(PointLight pointLight, Vector3 direction) {
	lightData->pointLight_ = pointLight;
	lightData->directionLight_.direction = Math::Normalize(direction);

}

void Model::SpotLightDraw(SpotLight spotLight) {
	lightData->spotLight_ = spotLight;
	lightData->spotLight_.direction_ = Math::Normalize(spotLight.direction_);
	
}

void Model::DirectionalLight(Vector4 color, Vector3 direction, float intensity) {

	lightData->directionLight_.color = color;
	lightData->directionLight_.direction = Math::Normalize(direction);
	lightData->directionLight_.intensity = intensity;

}

void Model::SetMaskTexture(const std::string& path) {
	TextureManager::GetInstance()->Load("resources/Mask/" + path);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/Mask/" + path);
}

void Model::SetTexture(const std::string& path) {
	if (path != "") {
		TextureManager::GetInstance()->Load("resources/" + path);
		texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/" + path);
	}
	
}

void Model::PostDraw() {}


