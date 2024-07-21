#include "HealerManager.h"

void HealerManager::Initialize() {
	

	Model_.reset(Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));

	for (int i = 0; i < 3; i++) {
		nHpModel_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Player/life0.png"));
		HpModel_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Player/life1.png"));
	}
	CaneModel_.reset(Model::CreateModelFromObj("resources/cane/cane.obj", "resources/cane/cane.png"));

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
	particle_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
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
	}
	else {
		particle_->StopParticles();
	}

	healer_->Update();
	healer_->followPlayer(playerPos_);
	healer_->searchTarget(enemyPos_);
};

void HealerManager::Draw(const ViewProjection& camera) {
	//Model_->Draw(healer_->GetWorldTransformHead(), camera, false);
	healer_->Draw(camera);
	

	particle_->Draw(camera);
	if (healer_->IsAttack()) {
		CaneModel_->Draw(healer_->GetWorldTransformCane(), camera, false);

	}

};

void HealerManager::RenderDirect(const ViewProjection& camera) {
	if (healer_->GetHitCount() >= 3) {
		HpModel_[0]->Draw(healer_->GetWorldTransfromHp(0), camera, false);
	}
	else {
		nHpModel_[0]->Draw(healer_->GetWorldTransfromHp(0), camera, false);
	}
	if (healer_->GetHitCount() >= 2) {
		HpModel_[1]->Draw(healer_->GetWorldTransfromHp(1), camera, false);
	}
	else {
		nHpModel_[1]->Draw(healer_->GetWorldTransfromHp(1), camera, false);
	}
	if (healer_->GetHitCount() >= 1) {
		HpModel_[2]->Draw(healer_->GetWorldTransfromHp(2), camera, false);
	}
	else {
		nHpModel_[2]->Draw(healer_->GetWorldTransfromHp(2), camera, false);
	}
};

void HealerManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

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