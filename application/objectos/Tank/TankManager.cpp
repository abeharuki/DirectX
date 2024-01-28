#include "TankManager.h"

void TankManager::Initialize() {

	Model_.reset(
	    Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));

	tank_ = std::make_unique<Tank>();
	tank_->Initialize();

	worldTransformBase_.Initialize();
	worldTransformBase_.translate.x = 2;
	worldTransformBase_.rotate.y = 3.14f;
	worldTransformBase_.UpdateMatrix();
}

void TankManager::Update() { 
	// 前のフレームの当たり判定のフラグを取得 
	preHit_ = isHit_;
	isHit_ = false;

	// 敵の判定
	preHitE_ = isHitE_;
	isHitE_ = false;

	tank_->Update();
	worldTransformBase_ = tank_->GetWorldTransform();
	tank_->followPlayer(playerPos_);
	tank_->searchTarget(enemyPos_);
};

void TankManager::Draw(const ViewProjection& camera) {
	Model_->Draw(worldTransformBase_, camera, false);
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

void TankManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

const WorldTransform& TankManager::GetWorldTransform() { return tank_->GetWorldTransform(); }
void TankManager::SetViewProjection(const ViewProjection& viewProjection) {
	tank_->SetViewProjection(viewProjection);
}