#include "Enemy.h"
#include <imgui.h>

void Enemy::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	// 初期化
	worldtransformBase_.Initialize();
	worldtransformBase_.translate = {0.0f, 0.0f, 100.0f};

	isHit_ = false;
	isDead_ = false;
}

void Enemy::Update() {

	BaseCharacter::Update();
	if (isHit_) {
		isDead_ = true;
	}

	worldtransformBase_.UpdateMatrix();

	ImGui::Begin("Enemy");
	
	ImGui::End();
}

void Enemy::Draw(const ViewProjection& viewprojection) {
	if (!isDead_) {
		models_[0]->Draw(worldtransformBase_, viewprojection);
	}
	
}

Vector3 Enemy::GetWorldPosition() { 
	Vector3 WorldPos; 
	WorldPos = worldtransformBase_.GetWorldPos();
	return WorldPos;
}
