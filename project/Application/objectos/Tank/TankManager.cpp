#include "TankManager.h"

void TankManager::Initialize() {
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
	Model_.reset(Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));
	for (int i = 0; i < 3; i++) {
		nHpModel_[i].reset(Model::CreateModelFromObj("resources/plane.obj", "resources/Player/life0.png"));
		HpModel_[i].reset(Model::CreateModelFromObj("resources/plane.obj", "resources/Player/life1.png"));
	}

	tank_ = std::make_unique<Tank>();
	tank_->Initialize();


}

void TankManager::Update() {
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

	tank_->Update();
	tank_->followPlayer(playerPos_);
	tank_->searchTarget(enemyPos_);
};

void TankManager::Draw(const ViewProjection& camera) {
	//Model_->Draw(tank_->GetWorldTransformHead(), camera, false);
	tank_->Draw(camera);
	if (tank_->GetHitCount() >= 3) {
		HpModel_[0]->Draw(tank_->GetWorldTransfromHp(0), camera, false);

	}
	else {
		nHpModel_[0]->Draw(tank_->GetWorldTransfromHp(0), camera, false);
	}
	if (tank_->GetHitCount() >= 2) {
		HpModel_[1]->Draw(tank_->GetWorldTransfromHp(1), camera, false);

	}
	else {
		nHpModel_[1]->Draw(tank_->GetWorldTransfromHp(1), camera, false);
	}
	if (tank_->GetHitCount() >= 1) {
		HpModel_[2]->Draw(tank_->GetWorldTransfromHp(2), camera, false);

	}
	else {
		nHpModel_[2]->Draw(tank_->GetWorldTransfromHp(2), camera, false);
	}
	particle_->Draw(camera);
};

// 衝突を検出したら呼び出されるコールバック関数
void TankManager::OnAllyCollision(const WorldTransform& worldTransform) {
	isHit_ = true;
	if (isHit_ != preHit_) {
		tank_->OnAllyCollision(worldTransform);
	}

}
// 衝突を検出したら呼び出されるコールバック関数
void TankManager::OnCollision(const WorldTransform& worldTransform) {
	isHitE_ = true;
	if (isHitE_ != preHitE_) {
		tank_->OnCollision(worldTransform);
	}
};


void TankManager::SetParticlePos(Vector3 pos) {
	particle_->SetTranslate(pos);
	isParticle_ = true;


}


void TankManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

const WorldTransform& TankManager::GetWorldTransform() { return tank_->GetWorldTransform(); }
void TankManager::SetViewProjection(const ViewProjection& viewProjection) {
	tank_->SetViewProjection(viewProjection);
}