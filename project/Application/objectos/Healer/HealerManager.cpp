#include "HealerManager.h"

void HealerManager::Initialize() {
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Healer", "Atlas.png", "healer.gltf"));

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
	worldTransformShadow_.scale = { 1.8f,1.8f,1.0f };

	healer_ = std::make_unique<Healer>();
	healer_->Initialize(animation_.get(),"Healer");

	worldTransformShadow_.translate = { healer_->GetWorldPosition().x,0.1f,healer_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();
}

void HealerManager::Update() {
	
	particle_->SetEmitter(emitter_);
	
	if (isParticle_) {
		particle_->Update();
		if (!particle_->GetEmit()) {
			isParticle_ = false;
		}
	}
	

	//影の計算
	shadowColor_.w = 1 - (healer_->GetWorldPosition().y / 3.9f);

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { healer_->GetWorldPosition().x,0.1f,healer_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	//キャラクターの更新
	healer_->Update();
	healer_->SetPlayerPos(playerPos_);

	
};

void HealerManager::Draw(const ViewProjection& camera) {
	healer_->Draw(camera);
	particle_->Draw(camera);
	
	shadowModel_->Draw(worldTransformShadow_, camera, false);

	if (healer_->IsAttack()) {
		StaffModel_->Draw(healer_->GetWorldTransformCane(), camera, false);
	}
	
}


void HealerManager::DrawUI(){
	healer_->DrawUI();
};

void HealerManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
}
