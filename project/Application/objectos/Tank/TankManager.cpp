#include "TankManager.h"

void TankManager::Initialize() {
	
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Tank", "Atlas.png", "tank.gltf"));

	shadowModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));
	shadowModel_->SetBlendMode(BlendMode::kSubtract);

	emitter_ = {
		.translate{0,0,0},
		.count{10},
		.frequency{0.02f},
		.frequencyTime{0.0f},
		.scaleRange{.min{1,1,1},.max{1,1,1}},
		.translateRange{.min{0,0,0},.max{0,0,0}},
		.colorRange{.min{1,1,1},.max{1,1,1}},
		.lifeTimeRange{.min{0.5f},.max{0.5f}},
		.velocityRange{.min{-0.2f,-0.2f,-0.2f},.max{0.2f,0.2f,0.2f}},
	};
	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));
	isParticle_ = false;

	worldTransformShadow_.Initialize();
	worldTransformShadow_.rotate = { -1.571f,0.0f,0.0f };
	worldTransformShadow_.scale = { 2.2f,2.2f,1.0f };

	tank_ = std::make_unique<Tank>();
	tank_->Initialize(animation_.get(),"Tank");

	worldTransformShadow_.translate = { tank_->GetWorldPosition().x,0.1f,tank_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();
}

void TankManager::Update() {
	particle_->SetEmitter(emitter_);
	if (isParticle_) {
		particle_->Update();
		if (!particle_->GetEmit()) {
			isParticle_ = false;
		}
	}
	
	//影の計算
	shadowColor_.w = 1 - (tank_->GetWorldPosition().y / 3.9f);

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { tank_->GetWorldPosition().x,0.1f,tank_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	//キャラクターの更新
	tank_->Update();
	tank_->SetPlayerPos(playerPos_);

	
	
};

void TankManager::Draw(const ViewProjection& camera) {
	tank_->Draw(camera);
	particle_->Draw(camera);

	shadowModel_->Draw(worldTransformShadow_, camera, false);
}
void TankManager::DrawUI(){
	tank_->DrawUI();
};

void TankManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
}

