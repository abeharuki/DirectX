#include "RenjuManager.h"

void RenjuManager::Initialize() {

	Model_.reset(
	    Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));

	renju_ = std::make_unique<Renju>();
	renju_->Initialize();

	worldTransformBase_.Initialize();
}

void RenjuManager::Update() { 
	renju_->Update();
	worldTransformBase_ = renju_->GetWorldTransform();
	renju_->followPlayer(playerPos_);

};

void RenjuManager::Draw(const ViewProjection& camera) {
	Model_->Draw(worldTransformBase_, camera, false);
};


void RenjuManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

const WorldTransform& RenjuManager::GetWorldTransform() { return renju_->GetWorldTransform(); }
void RenjuManager::SetViewProjection(const ViewProjection& viewProjection) {
	renju_->SetViewProjection(viewProjection);
}