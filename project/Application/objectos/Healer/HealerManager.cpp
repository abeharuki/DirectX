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

	
	emitter_ = {
		.translate{0,0,0},
		.count{1},
		.frequency{0.5f},
		.frequencyTime{0.5f},
		.scaleRange{.min{1,1,1},.max{1,1,1}},
		.translateRange{.min{0,0,0},.max{0,0,0}},
		.colorRange{.min{1,1,1},.max{1,1,1}},
		.velocityRange{.min{-0.2f,-0.2f,-0.2f},.max{0.2f,0.2f,0.2f}},
	};
	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));
	
	isParticle_ = false;

}

void HealerManager::Update() {
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
	

	healer_->Update();
	healer_->followPlayer(playerPos_);
	healer_->searchTarget(enemyPos_);
};

void HealerManager::Draw(const ViewProjection& camera) {
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
	emitter_.translate = pos;
	isParticle_ = true;
}



const WorldTransform& HealerManager::GetWorldTransform() { return healer_->GetWorldTransform(); }
void HealerManager::SetViewProjection(const ViewProjection& viewProjection) {
	healer_->SetViewProjection(viewProjection);
}