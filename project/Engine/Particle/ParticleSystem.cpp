#include "ParticleSystem.h"
#include "ImGuiManager.h"
#include <cassert>
#include <format>
#include <numbers>

ParticleSystem* ParticleSystem::instance_ = nullptr;

ParticleSystem* ParticleSystem::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ParticleSystem();
	}
	return instance_;
}

void ParticleSystem::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}

}


bool IsCollision(const AABB& aabb, const Vector3& point) {
	if ((aabb.min.x <= point.x && point.x <= aabb.max.x) &&
		(aabb.min.y <= point.y && point.y <= aabb.max.y) &&
		(aabb.min.z <= point.z && point.z <= aabb.max.z)) {
		return true;
	}

	return false;
}

void ParticleSystem::Initialize(const std::string& filename) {
	initializeCS_ = false;
	accelerationField_.acceleration = { 0.0f, 0.0f, 0.0f };
	LoadTexture(filename);
	CreateResource();
	sPipeline();
	worldTransform_.Initialize();
	linebox_ = std::make_unique<LineBox>();
	AABB aabb = { emitterSphere_->translateRange.min, emitterSphere_->translateRange.max };
	linebox_.reset(LineBox::Create(aabb));

}


void ParticleSystem::Update() {
	
	perFrame_->time = Engine::gameTime;
	emitterSphere_->frequencyTime += kDeltaTime;//タイムの加算
	//射出間隔を上回ったら射出許可を出して時間を調整
	if (emitterSphere_->frequency <= emitterSphere_->frequencyTime) {
		emitterSphere_->frequencyTime -= emitterSphere_->frequency;
		emitterSphere_->emit = 1;
	}
	else {
		emitterSphere_->emit = 0;
	}

	if (IsCollision(accelerationField_.area, emitterSphere_->translate)) {
		emitterSphere_->velocityRange.min -= accelerationField_.acceleration;
		emitterSphere_->velocityRange.max += accelerationField_.acceleration;
	}
	
	ImGui::Begin("GameTime");
	ImGui::Text("perFrame%f", perFrame_->time);
	ImGui::Text("gameTime%f", Engine::gameTime);
	ImGui::End();
	
}

void ParticleSystem::UpdatePerViewResource(const ViewProjection& viewProjection) {
	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = backToFrontMatrix * Math::Inverse(viewProjection.matView);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	perViewData_->view = viewProjection.matView;
	perViewData_->projection = viewProjection.matProjection;
	perViewData_->billboardMatrix = billboardMatrix;
}


void ParticleSystem::Draw(const ViewProjection& viewProjection) {


	//CSでの初期化と更新
	Engine::GetInstance()->TransitionResource(*particleResource_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	if (!initializeCS_) {
		InitilaizeCS();
		initializeCS_ = true;
	}
	UpdateCS();
	Engine::GetInstance()->TransitionResource(*particleResource_, D3D12_RESOURCE_STATE_GENERIC_READ);
	
#ifdef _DEBUG
	linebox_->Update({ emitterSphere_->translateRange.min, emitterSphere_->translateRange.max });
	worldTransform_.translate = emitterSphere_->translate;
	worldTransform_.UpdateMatrix();
#endif // DEBUG
	

	UpdatePerViewResource(viewProjection);

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &meshData_->GetVertexBufferView());

	//Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(1, particleResource_->GetSRVHandle());


	// wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialData_->GetResource()->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(3, perViewResource_->GetGPUVirtualAddress());


	Engine::GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(texture_));


	// 三角形の描画
	Engine::GetList()->DrawInstanced(6, kMaxParticles, 0, 0);

#ifdef _DEBUG

	linebox_->Draw(worldTransform_, viewProjection, false);

#endif // DEBUG
	
	

}

void ParticleSystem::SetFiled(AccelerationField accelerationField) {
	accelerationField_ = accelerationField;
}

void ParticleSystem::SetTexture(const std::string& filename) {
	TextureManager::GetInstance()->Load(filename);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(filename);
}

void ParticleSystem::SetModel(const std::string& filename, std::string& path) {
	modelData = ModelManager::LoadObjFile("resources/" + filename + path);
	if (modelData.material.textureFilePath != "") {
		TextureManager::GetInstance()->Load("resources/" + filename + modelData.material.textureFilePath);
		texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/" + filename + modelData.material.textureFilePath);
	}
}

ParticleSystem* ParticleSystem::Create(const std::string& filename) {
	ParticleSystem* model = new ParticleSystem;
	model->Initialize(filename);
	return model;
}

void ParticleSystem::InitilaizeCS() {

	Engine::GetList()->SetComputeRootSignature(CSRootSignature_.Get());
	Engine::GetList()->SetPipelineState(CSPipelineState_.Get());
	Engine::GetList()->SetComputeRootDescriptorTable(0, particleResource_->GetUAVHandle());
	Engine::GetList()->SetComputeRootDescriptorTable(1, freeListIndexResource_->GetUAVHandle());
	Engine::GetList()->SetComputeRootDescriptorTable(2, freeListResource_->GetUAVHandle());
	Engine::GetList()->Dispatch(1, 1, 1);


}

void ParticleSystem::UpdateCS() {

	Engine::GetList()->SetComputeRootSignature(emiteCSRootSignature_.Get());
	Engine::GetList()->SetPipelineState(emiteCSPipelineState_.Get());
	Engine::GetList()->SetComputeRootDescriptorTable(0, particleResource_->GetUAVHandle());
	Engine::GetList()->SetComputeRootConstantBufferView(1, emitterResource_->GetGPUVirtualAddress());
	Engine::GetList()->SetComputeRootConstantBufferView(2, perFrameResource_->GetGPUVirtualAddress());
	Engine::GetList()->SetComputeRootDescriptorTable(3, freeListIndexResource_->GetUAVHandle());
	Engine::GetList()->SetComputeRootDescriptorTable(4, freeListResource_->GetUAVHandle());
	Engine::GetList()->Dispatch(1, 1, 1);
	Engine::UAVBarrier(*particleResource_);//emiterCSの実行が終わってからupdateCSの実行に移る
	Engine::GetList()->SetComputeRootSignature(updateParticleCSRootSignature_.Get());
	Engine::GetList()->SetPipelineState(updateParticleCSPipelineState_.Get());
	Engine::GetList()->SetComputeRootDescriptorTable(0, particleResource_->GetUAVHandle());
	Engine::GetList()->SetComputeRootConstantBufferView(1, perFrameResource_->GetGPUVirtualAddress());
	Engine::GetList()->SetComputeRootDescriptorTable(2, freeListIndexResource_->GetUAVHandle());
	Engine::GetList()->SetComputeRootDescriptorTable(3, freeListResource_->GetUAVHandle());
	Engine::GetList()->Dispatch(1, 1, 1);

}

void ParticleSystem::CreateResource() {
	particleResource_ = std::make_unique<RWStructuredBuffer>();
	particleResource_->Create(kMaxParticles, sizeof(ParticleCS));

	freeListIndexResource_ = std::make_unique<RWStructuredBuffer>();
	freeListIndexResource_->Create(1, sizeof(int32_t));

	freeListResource_ = std::make_unique<RWStructuredBuffer>();
	freeListResource_->Create(kMaxParticles, sizeof(uint32_t));


	perViewResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(PerView));
	perViewResource_->Map(0, nullptr, reinterpret_cast<void**>(&perViewData_));

	emitterResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(EmitterSphere));
	emitterResource_->Map(0, nullptr, reinterpret_cast<void**>(&emitterSphere_));
	emitterSphere_->count = 10;
	emitterSphere_->frequency = 0.5f;
	emitterSphere_->frequencyTime = 0.0f;
	emitterSphere_->translate = Vector3(0.0f, 0.0f, 0.0f);
	emitterSphere_->radius = 1.0f;
	emitterSphere_->emit = 0;
	emitterSphere_->scaleRange = { .min{1,1,1},.max{1,1,1} };
	emitterSphere_->translateRange = { .min{0,0,0},.max{0,0,0} };
	emitterSphere_->colorRange = { .min{1,1,1},.max{1,1,1} };
	emitterSphere_->velocityRange = { .min{-0.1f,-0.1f,-0.1f},.max{0.1f,0.1f,0.1f} };

	perFrameResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(EmitterSphere));
	perFrameResource_->Map(0, nullptr, reinterpret_cast<void**>(&perFrame_));
	perFrame_->time = 0;
	perFrame_->deltaTime = 1.0f / 60.0f;


	CreateVertexResource();

}

// 頂点データの設定
void ParticleSystem::CreateVertexResource() {
	meshData_ = std::make_unique<Mesh>();
	meshData_->Initialize(modelData);

	materialData_ = std::make_unique<Material>();
	materialData_->Initialize();
};

void ParticleSystem::LoadTexture(const std::string& filename) {
	modelData = ModelManager::LoadObjFile("resources/particle/plane.obj");
	TextureManager::GetInstance()->Load(filename);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(filename);

}

void ParticleSystem::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateParticleVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreateParticlePSShader();

	rootSignature_ = GraphicsPipeline::GetInstance()->CreateParticleRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateParticleGraphicsPipeline(blendMode_);

	GraphicsPipeline::GetInstance()->CreateParticleCSShader();
	CSRootSignature_ = GraphicsPipeline::GetInstance()->CreateParticleCSRootSignature();
	CSPipelineState_ = GraphicsPipeline::GetInstance()->CreateParticleCSGraphicsPipeline();

	GraphicsPipeline::GetInstance()->CreateEmiteCSShader();
	emiteCSRootSignature_ = GraphicsPipeline::GetInstance()->CreateEmiteCSRootSignature();
	emiteCSPipelineState_ = GraphicsPipeline::GetInstance()->CreateEmiteCSGraphicsPipeline();

	GraphicsPipeline::GetInstance()->CreateUpdateParticleCSShader();
	updateParticleCSRootSignature_ = GraphicsPipeline::GetInstance()->CreateUpdateParticleCSRootSignature();
	updateParticleCSPipelineState_ = GraphicsPipeline::GetInstance()->CreateUpdateParticleCSGraphicsPipeline();

};