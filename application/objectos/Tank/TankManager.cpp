#include "TankManager.h"

void TankManager::Initialize() {

	Model_.reset(
	    Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));

	tank_ = std::make_unique<Tank>();
	tank_->Initialize();

	worldTransformBase_.Initialize();
	worldTransformBase_.translate.x = 2;
}

void TankManager::Update() { 
	tank_->Update();
	worldTransformBase_ = tank_->GetWorldTransform();
	tank_->followPlayer(playerPos_);
	tank_->searchTarget(enemyPos_);
};

void TankManager::Draw(const ViewProjection& camera) {
	Model_->Draw(worldTransformBase_, camera, false);
};



void TankManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

const WorldTransform& TankManager::GetWorldTransform() { return tank_->GetWorldTransform(); }
void TankManager::SetViewProjection(const ViewProjection& viewProjection) {
	tank_->SetViewProjection(viewProjection);
}