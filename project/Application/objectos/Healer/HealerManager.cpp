#include "HealerManager.h"

void HealerManager::Initialize() {

	Model_.reset(
	    Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));

	CaneModel_.reset( Model::CreateModelFromObj("resources/cane/cane.obj", "resources/cane/cane.png"));


	healer_ = std::make_unique<Healer>();
	healer_->Initialize();

	emitter_.transform = {
	    {0.8f, 0.8f, 0.8f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
	emitter_.count = 10;
	emitter_.frequencyTime = 0;

	particle_.reset(Particle::Create("resources/particle/circle.png", emitter_));
	particle_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	particle_->SetSpeed(5.0f);
	isParticle_ = false;
	
}

void HealerManager::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	// 敵の判定
	preHitE_ = isHitE_;
	isHitE_ = false;
	if (isParticle_) {
		particle_->Update();
		isParticle_ = false;
	} else {
		particle_->StopParticles();
	}

	healer_->Update();
	healer_->followPlayer(playerPos_);
	healer_->searchTarget(enemyPos_);
};

void HealerManager::Draw(const ViewProjection& camera) {
	Model_->Draw(healer_->GetWorldTransformHead(), camera, false);
	particle_->Draw(camera);
	if (healer_->IsAttack()) {
		CaneModel_->Draw(healer_->GetWorldTransformCane(), camera, false);
	
	}
	
};

void HealerManager::followPlayer(Vector3 playerPos) {playerPos_ = playerPos;}

void HealerManager::OnAllyCollision(const WorldTransform& worldTransform) {
	isHit_ = true;
	if (isHit_ != preHit_) {
		healer_->OnAllyCollision(worldTransform);
	}
	
	
}



void HealerManager::SetParticlePos(Vector3 pos) {
	particle_->SetTranslate(pos);
	isParticle_ = true;
}



const WorldTransform& HealerManager::GetWorldTransform() { return healer_->GetWorldTransform(); }
 void HealerManager::SetViewProjection(const ViewProjection& viewProjection) {
	healer_->SetViewProjection(viewProjection);
}