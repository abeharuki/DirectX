#include "Enemy.h"

void Enemy::Initialize() {

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.rotate.x = 1.65f;
	worldTransformBase_.translate.z = 10.0f;
}

void Enemy::Update() { 
	worldTransformBase_.UpdateMatrix();


	ImGui::Begin("Enemy");
	ImGui::SliderFloat3("roto", &worldTransformBase_.rotate.x, -1.0f, 5.0f);
	ImGui::End();

}

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

Enemy::~Enemy() {}