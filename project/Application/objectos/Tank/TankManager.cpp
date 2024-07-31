#include "TankManager.h"

void TankManager::Initialize() {


	
	emitter_ = {
		.translate{0,0,0},
		.count{5},
		.frequency{0.5f},
		.frequencyTime{0.5f},
		.scaleRange{.min{1,1,1},.max{1,1,1}},
		.translateRange{.min{0,0,0},.max{0,0,0}},
		.colorRange{.min{1,1,1},.max{1,1,1}},
		.lifeTimeRange{.min{0.5f},.max{0.5f}},
		.velocityRange{.min{-0.2f,-0.2f,-0.2f},.max{0.2f,0.2f,0.2f}},
	};
	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));
	Model_.reset(Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));
	isParticle_ = false;
	for (int i = 0; i < 3; i++) {
		nHpModel_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Player/life0.png"));
		HpModel_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Player/life1.png"));
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
	
	

	tank_->Update();
	tank_->followPlayer(playerPos_);
	tank_->searchTarget(enemyPos_);
};

void TankManager::Draw(const ViewProjection& camera) {
	tank_->Draw(camera);
	particle_->Draw(camera);
};

void TankManager::RenderDirect(const ViewProjection& camera) {
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
}

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
	emitter_.translate = pos;
	isParticle_ = true;
}


void TankManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

const WorldTransform& TankManager::GetWorldTransform() { return tank_->GetWorldTransform(); }
void TankManager::SetViewProjection(const ViewProjection& viewProjection) {
	tank_->SetViewProjection(viewProjection);
}