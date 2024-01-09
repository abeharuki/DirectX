#include "RenjuManager.h"

void RenjuManager::Initialize() {

	Model_.reset(
	    Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));

	renju_ = std::make_unique<Renju>();
	renju_->Initialize();

	worldTransformBase_.Initialize();
}

void RenjuManager::Update() { 
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	renju_->Update();
	worldTransformBase_ = renju_->GetWorldTransform();
	renju_->followPlayer(playerPos_);
	renju_->searchTarget(enemyPos_);
};

void RenjuManager::Draw(const ViewProjection& camera) {
	Model_->Draw(worldTransformBase_, camera, false);
	renju_->Draw(camera);
};


// 衝突を検出したら呼び出されるコールバック関数
void RenjuManager::OnAllyCollision(const WorldTransform& worldTransform){
	isHit_ = true;
	if (isHit_ != preHit_) {
			renju_->OnAllyCollision(worldTransform);
	}

};


void RenjuManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

const WorldTransform& RenjuManager::GetWorldTransform() { return renju_->GetWorldTransform(); }
void RenjuManager::SetViewProjection(const ViewProjection& viewProjection) {
	renju_->SetViewProjection(viewProjection);
}