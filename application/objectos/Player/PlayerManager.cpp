#include "PlayerManager.h"

void PlayerManager::Initialize() {

	Model_.reset(Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));
	HammerModel_.reset(Model::CreateModelFromObj("resources/katana/katana.obj",  "resources/katana/kata.png"));

	player_ = std::make_unique<Player>();
	player_->Initialize();

	
	
}

void PlayerManager::Update() {
	player_->Update();

	
};

void PlayerManager::Draw(const ViewProjection& camera) {
	Model_->Draw(player_->GetWorldTransformHead(), camera, false);

	HammerModel_->Draw(player_->GetWorldTransformHammer(), camera, false);
};

const WorldTransform& PlayerManager::GetWorldTransform() { return player_->GetWorldTransform(); }
Vector3 PlayerManager::GetWorldPos() { return player_->GetWorldTransform().GetWorldPos(); }
void PlayerManager::SetViewProjection(const ViewProjection* viewProjection) {
	player_->SetViewProjection(viewProjection);
}