#include "Particle.h"
#include <cassert>
#include <format>

std::random_device seedGenerator;
std::mt19937 randomEngine(seedGenerator());

void Particle::Initialize(const std::string& filename, uint32_t Count) {
	instanceCount = Count;
	transforms[Count];
	LoadTexture(filename);
	CreateVertexResource();
	sPipeline();
	CreateInstanceSRV();
}

void Particle::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateParticleVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreateParticlePSShader();

	rootSignature_ = GraphicsPipeline::GetInstance()->CreateParticleRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateParticleGraphicsPipeline(blendMode_);
};

void Particle::Update() { 
	for (uint32_t i = 0; i < instanceCount; ++i){
		transforms[i].transform.translate += transforms[i].velocity* kDeltaTime;
	} 
}

void Particle::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection) {
	

	for (uint32_t i = 0; i < instanceCount; ++i) {
		transforms[i].transform.scale = worldTransform.scale;
		transforms[i].transform.rotate = worldTransform.rotate;
		transforms[i].transform.translate = transforms[i].transform.translate + worldTransform.translate;
	}

	for (uint32_t i = 0; i < instanceCount; ++i) {
		Matrix4x4 worldMatrix = Math::MakeAffineMatrix(
		    transforms[i].transform.scale, transforms[i].transform.rotate, transforms[i].transform.translate);
		Matrix4x4 worldViewProjectionMatrixSprite = Math::Multiply(worldMatrix,Math::Multiply(viewProjection.matView, viewProjection.matProjection));
		instancingData[i].WVP = worldViewProjectionMatrixSprite;
		instancingData[i].World = worldMatrix;

	}
	

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(1, instancingSrvHandelGPU);
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texture_));

	// wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView( 0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView( 3, instancingResouce_->GetGPUVirtualAddress());
	
	// 三角形の描画
	Engine::GetList()->DrawInstanced(UINT(modelData.vertices.size()), instanceCount, 0, 0);
}

// 頂点データの設定
void Particle::CreateVertexResource() {
	instancingResouce_ = Mesh::CreateBufferResoure(
	    Engine::GetDevice().Get(), sizeof(TransformationMatrix) * instanceCount);
	// データを書き込む
	instancingData = nullptr;
	// 書き込むためのアドレスを取得
	instancingResouce_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	// 単位行列を書き込む
	for (uint32_t i = 0; i < instanceCount; ++i) {
		instancingData[i].WVP = Math::MakeIdentity4x4();
		instancingData[i].World = Math::MakeIdentity4x4();
	}


	// 頂点リソースを作る
	vertexResource_ = Mesh::CreateBufferResoure(
	    Engine::GetDevice().Get(), sizeof(VertexData) * modelData.vertices.size());
	vertexResource_->Map(
	    0, nullptr, reinterpret_cast<void**>(&vertexData_)); // 書き込むためのアドレスを取得

	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation =
	    vertexResource_->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(
	    sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	std::memcpy(
	    vertexData_, modelData.vertices.data(),
	    sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースにコピース

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
	// 初期化
	materialData->uvTransform = Math::MakeIdentity4x4();
	
	for (uint32_t i = 0; i < instanceCount; ++i) {
		transforms[i].transform.scale = {1.0f,1.0f,1.0f};
		transforms[i].transform.rotate = {0.0f,0.0f,0.0f};
		transforms[i].transform.translate = {(i * 0.1f ), (i * 0.1f),(i * 0.1f )};
		transforms[i].velocity = {0.0f, 1.0f, 0.0f};
	}
	
};

void Particle::SetColor(Vector4 color) {
	materialData->color.rgb = {color.x, color.y, color.z};
	materialData->color.a = color.w;
}

void Particle::SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }

Particle* Particle::Create(const std::string& filename,uint32_t Count) {
	Particle* model = new Particle;
	model->Initialize(filename,Count);
	return model;
}

void Particle::LoadTexture(const std::string& filename) {
	modelData = TextureManager::LoadObjFile("resources/plane.obj");
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();
	texture_ = textureManager_->Load(filename);
}

void Particle::CreateInstanceSRV() {
	descriptorSizeSRV = Engine::GetDevice()->GetDescriptorHandleIncrementSize(
	    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = instanceCount;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
	instancingSrvHandelCPU = Engine::GetCPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV,2);
	instancingSrvHandelGPU = Engine::GetGPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV,2);
	Engine::GetDevice()->CreateShaderResourceView(
	    instancingResouce_.Get(), &instancingSrvDesc, instancingSrvHandelCPU);

}

