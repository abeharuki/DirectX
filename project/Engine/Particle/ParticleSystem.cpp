#include "ParticleSystem.h"
#include "ImGuiManager.h"
#include <cassert>
#include <format>
#include <numbers>

bool IsCollision(const AABB& aabb, const Vector3& point) {
	if ((aabb.min.x <= point.x && point.x <= aabb.max.x) &&
		(aabb.min.y <= point.y && point.y <= aabb.max.y) &&
		(aabb.min.z <= point.z && point.z <= aabb.max.z)) {
		return true;
	}

	return false;
}

void ParticleSystem::Initialize(const std::string& filename, Emitter emitter) {
	emitter_ = emitter;
	accelerationField_.acceleration = { 0.0f, 0.0f, 0.0f };
	LoadTexture(filename);
	CreateVertexResource();
	CreateInstanceSRV();
	sPipeline();
	
}


void ParticleSystem::Update() {
	particle = true; 
	// 乱数生成
	std::mt19937 randomEngine(seedGenerator());
	emitter_.frequencyTime += kDeltaTime;
	if (particle) {
		if (emitter_.frequency <= emitter_.frequencyTime) {
			particles.splice(particles.end(), Emission(emitter_, randomEngine));
			emitter_.frequencyTime -= emitter_.frequency;
		}
	}
}

void ParticleSystem::StopParticles() { particle = false; }

void ParticleSystem::Draw(const ViewProjection& viewProjection) {
	
	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = backToFrontMatrix * Math::Inverse(viewProjection.matView);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	uint32_t numInstance = 0;

	for (std::list<Particle>::iterator particleIterator = particles.begin();particleIterator != particles.end();) {

		if ((*particleIterator).lifeTime < (*particleIterator).currentTime) {
			particleIterator = particles.erase(particleIterator);
			continue;
		}

		if (IsCollision(accelerationField_.area, (*particleIterator).transform.translate)) {
			(*particleIterator).velocity += accelerationField_.acceleration * kDeltaTime;
		}

		(*particleIterator).transform.translate += (*particleIterator).velocity * kDeltaTime;
		(*particleIterator).currentTime += kDeltaTime;
		float alph = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

		if (numInstance < kNumMaxInstance) {
			Matrix4x4 worldMatrix =
				Math::MakeScaleMatrix((*particleIterator).transform.scale) * billboardMatrix *
				Math::MakeTranslateMatrix((*particleIterator).transform.translate);

			instancingData[numInstance].World = worldMatrix;
			instancingData[numInstance].color = (*particleIterator).color;
			instancingData[numInstance].color.w = alph;
			++numInstance;
		}

		++particleIterator;
	}

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &meshData_->GetVertexBufferView());

	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(1, TextureManager::GetInstance()->GetParticleGPUHandle(instancing_));

	// wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(
		0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(
		4, viewProjection.constBuff_->GetGPUVirtualAddress());

	Engine::GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(texture_));


	// 三角形の描画
	Engine::GetList()->DrawInstanced(UINT(meshData_->GetVerticesSize()), numInstance, 0, 0);
}



void ParticleSystem::SetSpeed(float speed) { kDeltaTime = speed / 60.0f; }

void ParticleSystem::SetColor(Vector4 color) {
	color_ = color;
	isColor = true;
}

void ParticleSystem::SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }

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

ParticleSystem* ParticleSystem::Create(const std::string& filename, Emitter emitter) {
	ParticleSystem* model = new ParticleSystem;
	model->Initialize(filename, emitter);
	return model;
}

// 頂点データの設定
void ParticleSystem::CreateVertexResource() {
	instancingResouce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(ParticleForGPU) * kNumMaxInstance);
	// データを書き込む
	instancingData = nullptr;
	// 書き込むためのアドレスを取得
	instancingResouce_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	// 単位行列を書き込む
	for (uint32_t i = 0; i < kNumMaxInstance; ++i) {
		instancingData[i].World = Math::MakeIdentity4x4();
		instancingData[i].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	meshData_ = std::make_unique<Mesh>();
	meshData_->Initialize(modelData);

	// マテリアル
	materialResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(MaterialD));

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
};
void ParticleSystem::CreateInstanceSRV() {

	instancing_ = TextureManager::GetInstance()->ParticleLoad(instancingResouce_.Get(), kNumMaxInstance);
}

void ParticleSystem::LoadTexture(const std::string& filename) {
	modelData = ModelManager::LoadObjFile("resources/particle/plane.obj");
	TextureManager::GetInstance()->Load(filename);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(filename);

}


Particle ParticleSystem::MakeNewParticle(std::mt19937& randomEngine, const Transform transform) {
	std::uniform_real_distribution<float> distribution(-1.0, 1.0);
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0, 3.0);
	Particle particle;
	particle.transform.scale = transform.scale;
	particle.transform.rotate = transform.rotate;
	Vector3 randomTranslate{ distribution(randomEngine) ,distribution(randomEngine) ,distribution(randomEngine) };
	particle.transform.translate = transform.translate + randomTranslate;
	particle.velocity = {distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
	if (isColor) {
		particle.color = color_;
	}
	else {
		particle.color = {distColor(randomEngine), distColor(randomEngine), distColor(randomEngine),1.0f };
	}
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;
	return particle;
}

std::list<Particle> ParticleSystem::Emission(const Emitter& emitter, std::mt19937& randomEngine) {
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeNewParticle(randomEngine, emitter.transform));
	}
	return particles;
}

void ParticleSystem::UpdateBillboard(const ViewProjection& viewProjection){
}

void ParticleSystem::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateParticleVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreateParticlePSShader();

	rootSignature_ = GraphicsPipeline::GetInstance()->CreateParticleRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateParticleGraphicsPipeline(blendMode_);
};
