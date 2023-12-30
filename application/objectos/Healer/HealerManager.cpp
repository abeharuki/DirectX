#include "HealerManager.h"

void HealerManager::Initialize() {

	Model_.reset(
	    Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));

	healer_ = std::make_unique<Healer>();
	healer_->Initialize();

	worldTransformBase_.Initialize();
	worldTransformBase_.translate.x = 2;
	
}

void HealerManager::Update() {
	healer_->Update();
	worldTransformBase_ = healer_->GetWorldTransform();
	healer_->followPlayer(playerPos_);
};

void HealerManager::Draw(const ViewProjection& camera) {
	Model_->Draw(worldTransformBase_, camera, false);
};

void HealerManager::followPlayer(Vector3 playerPos) {playerPos_ = playerPos;}


const WorldTransform& HealerManager::GetWorldTransform() { return healer_->GetWorldTransform(); }
 void HealerManager::SetViewProjection(const ViewProjection& viewProjection) {
	healer_->SetViewProjection(viewProjection);
}