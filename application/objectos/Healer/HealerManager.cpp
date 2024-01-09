#include "HealerManager.h"

void HealerManager::Initialize() {

	Model_.reset(
	    Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));

	CaneModel_.reset( Model::CreateModelFromObj("resources/cane/cane.obj", "resources/cane/cane.png"));


	healer_ = std::make_unique<Healer>();
	healer_->Initialize();

	
	
}

void HealerManager::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	healer_->Update();
	healer_->followPlayer(playerPos_);
	healer_->searchTarget(enemyPos_);
};

void HealerManager::Draw(const ViewProjection& camera) {
	Model_->Draw(healer_->GetWorldTransform(), camera, false);
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

const WorldTransform& HealerManager::GetWorldTransform() { return healer_->GetWorldTransform(); }
 void HealerManager::SetViewProjection(const ViewProjection& viewProjection) {
	healer_->SetViewProjection(viewProjection);
}