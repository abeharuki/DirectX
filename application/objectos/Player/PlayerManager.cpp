#include "PlayerManager.h"

void PlayerManager::Initialize() {

	Model_.reset(Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));
	HammerModel_.reset(Model::CreateModelFromObj("resources/katana/katana.obj",  "resources/katana/kata.png"));
	bulletModel_.reset(
	    Model::CreateModelFromObj("resources/Renju/cube.obj", "resources/Renju/Bullet.png"));

	player_ = std::make_unique<Player>();
	player_->Initialize();

	
	
}

void PlayerManager::Update() {
	player_->Update();

	
};

void PlayerManager::Draw(const ViewProjection& camera) {
	Model_->Draw(player_->GetWorldTransformHead(), camera, false);

	if (player_->IsAttack()) {
		HammerModel_->Draw(player_->GetWorldTransformHammer(), camera, false);

	}
	
};

// 衝突を検出したら呼び出されるコールバック関数
void PlayerManager::OnAllyCollision(const WorldTransform& worldTransform){
};


const WorldTransform& PlayerManager::GetWorldTransform() { return player_->GetWorldTransform(); }
Vector3 PlayerManager::GetWorldPos() { return player_->GetWorldTransform().GetWorldPos(); }
Vector3 PlayerManager::katanaPos() { return player_->GetWorldTransformCollision().GetWorldPos(); }
void PlayerManager::SetViewProjection(const ViewProjection* viewProjection) {
	player_->SetViewProjection(viewProjection);
}