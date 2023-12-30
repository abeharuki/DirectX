#include "EnemyManager.h"

void EnemyManager::Initialize() {

	Model_.reset( Model::CreateModelFromObj("resources/enemy/enemy.obj", "resources/enemy/enemy.png"));

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	worldTransformBase_.Initialize();
}

void EnemyManager::Update() { enemy_->Update(); };

void EnemyManager::Draw(const ViewProjection& camera) {
	Model_->Draw(enemy_->GetWorldTransform(), camera, false);
};

Vector3 EnemyManager::GetWorldPos() { return enemy_->GetWorldTransform().GetWorldPos(); }

const WorldTransform& EnemyManager::GetWorldTransform() { return enemy_->GetWorldTransform(); }