#include "EnemyManager.h"

void EnemyManager::Initialize() {

	Model_.reset( Model::CreateModelFromObj("resources/enemy/enemy.obj", "resources/enemy/enemy.png"));
	bulletModel_.reset( Model::CreateModelFromObj("resources/Renju/cube.obj", "resources/Renju/Bullet.png"));
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	worldTransformBase_.Initialize();
	worldTransform_.Initialize();
	worldTransform_ = worldTransformBase_;
}

void EnemyManager::Update() { 
	enemy_->Update(); 

	worldTransform_.UpdateMatrix();

	ImGui::Begin("enemy");
	ImGui::SliderFloat3("size", &worldTransform_.scale.x, 0.0f, 5.0f);
	ImGui::SliderFloat3("pos", &worldTransform_.translate.x, 0.0f, 5.0f);
	ImGui::End();
};

void EnemyManager::Draw(const ViewProjection& camera) {
	Model_->Draw(enemy_->GetWorldTransform(), camera, false);
	bulletModel_->Draw(worldTransform_, camera, false);
};

// 衝突を検出したら呼び出されるコールバック関数
void EnemyManager::OnCollision(){ 
	
	ImGui::Begin("enemy");
	ImGui::Text("Hit");
	ImGui::End();


};

Vector3 EnemyManager::GetWorldPos() { return enemy_->GetWorldTransform().GetWorldPos(); }

const WorldTransform& EnemyManager::GetWorldTransform() { return enemy_->GetWorldTransform(); }