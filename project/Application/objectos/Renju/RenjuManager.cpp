#include "RenjuManager.h"

void RenjuManager::Initialize() {

	Model_.reset(Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));
	for (int i = 0; i < 3; i++) {
		nHpModel_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Player/life0.png"));
		HpModel_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Player/life1.png"));
	}

	renju_ = std::make_unique<Renju>();
	renju_->Initialize();
	emitter_ = {
		.translate{0,0,0},
		.count{10},
		.frequency{0.5f},
		.frequencyTime{0.5f},
		.scaleRange{.min{1,1,1},.max{1,1,1}},
		.translateRange{.min{0,0,0},.max{0,0,0}},
		.colorRange{.min{1,1,1},.max{1,1,1}},
		.velocityRange{.min{-0.2f,-0.2f,-0.2f},.max{0.2f,0.2f,0.2f}},
	};
	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));
	
}

void RenjuManager::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	// 敵の判定
	preHitE_ = isHitE_;
	isHitE_ = false;

	particle_->SetEmitter(emitter_);
	particle_->Update();
	if (isParticle_) {
		particle_->SetFrequencyTime(0.5f);
		if (particle_->GetEmit()) {
			isParticle_ = false;
		}
	}
	else {
		particle_->StopParticle();
	}

	renju_->Update();
	renju_->followPlayer(playerPos_);
	renju_->searchTarget(enemyPos_);
};

void RenjuManager::Draw(const ViewProjection& camera) {
	renju_->Draw(camera);
	particle_->Draw(camera);
};

void RenjuManager::RenderDirect(const ViewProjection& camera) {
	if (renju_->GetHitCount() >= 3) {
		HpModel_[0]->Draw(renju_->GetWorldTransfromHp(0), camera, false);

	}
	else {
		nHpModel_[0]->Draw(renju_->GetWorldTransfromHp(0), camera, false);
	}
	if (renju_->GetHitCount() >= 2) {
		HpModel_[1]->Draw(renju_->GetWorldTransfromHp(1), camera, false);

	}
	else {
		nHpModel_[1]->Draw(renju_->GetWorldTransfromHp(1), camera, false);
	}
	if (renju_->GetHitCount() >= 1) {
		HpModel_[2]->Draw(renju_->GetWorldTransfromHp(2), camera, false);

	}
	else {
		nHpModel_[2]->Draw(renju_->GetWorldTransfromHp(2), camera, false);
	}
}

void RenjuManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
}

// 衝突を検出したら呼び出されるコールバック関数
void RenjuManager::OnAllyCollision(const WorldTransform& worldTransform) {
	isHit_ = true;
	if (isHit_ != preHit_) {
		renju_->OnAllyCollision(worldTransform);
	}

};

// 衝突を検出したら呼び出されるコールバック関数
void RenjuManager::OnCollision(const WorldTransform& worldTransform) {
	isHitE_ = true;
	if (isHitE_ != preHitE_) {
		renju_->OnCollision(worldTransform);
	}
};
void RenjuManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

const WorldTransform& RenjuManager::GetWorldTransform() { return renju_->GetWorldTransform(); }
void RenjuManager::SetViewProjection(const ViewProjection& viewProjection) {
	renju_->SetViewProjection(viewProjection);
}