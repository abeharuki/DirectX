#include "Model.h"
#include <cassert>
#include <format>
#include <imgui.h>


Microsoft::WRL::ComPtr<ID3D12Resource> Model::lightResource_;
WritingStyle* Model::lightData;

void Model::Initialize(const std::string& filename, const std::string& texturePath) { 
	
	LoadTexture(filename, texturePath);
	CreateVertexResource();
	sPipeline();
	
	
}



void Model::sPipeline() {
	
	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePSShader();
	
	
	rootSignature_ = GraphicsPipeline::GetInstance()->CreateRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateGraphicsPipeline(blendMode_);

	



	
};


void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection ,bool light) {
	
	//カメラpos
	cameraData->worldPos = viewProjection.worldPos_;

	//ライティング有効化
	materialData->enableLighting = light;

	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateGraphicsPipeline(blendMode_);


	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	
	

	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(texture_));

    // wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(5, cameraResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());

	// 三角形の描画
	Engine::GetList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

	
}

//頂点データの設定
void Model::CreateVertexResource() {
	// モデルの読み込み 
	// 頂点リソースを作る
	vertexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) * modelData.vertices.size());
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得

	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation =vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(
	    sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	
	std::memcpy( vertexData, modelData.vertices.data(),sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースにコピース
	
	

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

	// ライティング
	lightResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(WritingStyle));
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

	// デフォルト値
	lightData->directionLight_.color = {1.0f, 1.0f, 1.0f, 1.0f};
	lightData->directionLight_.direction = {0.0f, -1.0f, 0.0f};
	lightData->directionLight_.intensity = 1.0f;
	//lightData->directionLight_.isEnable_ = true;
	

	//カメラ
	cameraResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(CameraForGPU));
	cameraResorce_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

};

void Model::SetColor(Vector4 color) {
	materialData->color.rgb = {color.x, color.y, color.z};
	materialData->color.a = color.w;
}

void Model::SetBlendMode(BlendMode blendMode) { 
	blendMode_ = blendMode; 
	

}

void Model::SetShininess(float i) { materialData->shininess = i; }


Model* Model::CreateModelFromObj(const std::string& filename, const std::string& texturePath) {
	Model* model = new Model;
	model->Initialize(filename, texturePath);
	return model;


}

void Model::LoadTexture(const std::string& filename, const std::string& texturePath) {
	modelData = ModelManager::LoadObjFile(filename);
	TextureManager::GetInstance()->Load(texturePath);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(texturePath);
	//textureManager_->Initialize();
	//texture_ = textureManager_->Load(texturePath);
}


void Model::DirectionalLightDraw(DirectionLight directionLight) {
	lightData->directionLight_.color = directionLight.color;
	lightData->directionLight_.direction = Math::Normalize(directionLight.direction);
	lightData->directionLight_.intensity = directionLight.intensity;
	lightData->directionLight_.isEnable_ = true;
	lightData->pointLight_.isEnable_ = false;
	lightData->spotLight_.isEnable_ = false;

	

}
void Model::PointLightDraw(PointLight pointLight, Vector3 direction) {
	lightData->pointLight_ = pointLight;
	lightData->directionLight_.direction = Math::Normalize(direction);
	lightData->pointLight_.isEnable_ = true;
	lightData->spotLight_.isEnable_ = false;
	lightData->directionLight_.isEnable_ = false;
	lightData->directionLight_.intensity = 0.0f;

}
void Model::SpotLightDraw(SpotLight spotLight) {
	lightData->spotLight_ = spotLight;
	lightData->spotLight_.direction_ = Math::Normalize(spotLight.direction_);
	lightData->spotLight_.isEnable_ = true;
	lightData->pointLight_.isEnable_ = false;
	lightData->directionLight_.isEnable_ = false;
	
}

void Model::DirectionalLight(Vector4 color, Vector3 direction, float intensity) {

	lightData->directionLight_.color = color;
	lightData->directionLight_.direction = Math::Normalize(direction);
	lightData->directionLight_.intensity = intensity;

}

void Model::PostDraw() {}


