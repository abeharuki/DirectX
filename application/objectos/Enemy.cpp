#include "Enemy.h"
#include <Player.h>
#include <imgui.h>

void Enemy::Initialize(Vector3 pos) {
	modelEnemy_.reset(Model::CreateModelFromObj("resources/cube.obj", "resources/enemy.png"));
	std::vector<Model*> enemyModels = {modelEnemy_.get()};
	// 基底クラスの初期化
	BaseCharacter::Initialize(enemyModels);

	// 初期化
	worldtransformBase_.Initialize();
	worldtransformBase_.translate = pos;

	isHit_ = false;
	isDead_ = false;
}

void Enemy::Update() {

	BaseCharacter::Update();

	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;

	if (isPlayerFinalAttack) {
		hitCount = 0;
	}

	if (isDead_) {
		a -= 0.01f;
		worldtransformBase_.translate = Math::Add(worldtransformBase_.translate, deathVelocity);

	} else {
		Move();
	}
	if (a <= 0) {
		worldtransformBase_.translate.y = 100;
	}

	models_[0]->SetColor({1.0f, 1.0f, 1.0f, a});
	worldtransformBase_.UpdateMatrix();

	ImGui::Begin("Enemy");
	ImGui::Text("attack%d", hitCount);
	ImGui::DragFloat4("translation", &worldtransformBase_.translate.x, 0.01f);
	ImGui::End();
}

void Enemy::Draw(const ViewProjection& viewprojection,bool light) {
	if (a >= 0) {
		models_[0]->Draw(worldtransformBase_, viewprojection,light);
	}
}

void Enemy::OnCollision(const WorldTransform& worldTransform) {
	isHit_ = true;
	if (isHit_ != preHit_) {
		hitCount++;
	}

	if (hitCount >= Player::ComboNum) {
		isDead_ = true;
		const float kSpeed = 1.0f;
		deathVelocity = {0.0f, 0.0f, kSpeed};
		deathVelocity = Math::TransformNormal(deathVelocity, worldTransform.matWorld_);
	}
}

Vector3 Enemy::GetWorldPosition() {
	Vector3 WorldPos;
	WorldPos = worldtransformBase_.GetWorldPos();
	return WorldPos;
}

// 中心座標取得
Vector3 Enemy::GetSenterPosition() const {
	// 中心点のオフセット
	const Vector3 offset = {0.0f, 1.0f, 0.0f};
	// ワールド座標に変換
	Vector3 worldPos = Math::Transform(offset, worldtransformBase_.matWorld_);

	return worldPos;
}

void Enemy::Move() {
	const float kCharacterSpeed = 0.05f;
	Vector3 move = {0.0f, 0.0f, kCharacterSpeed};
	worldtransformBase_.rotate.y += 0.05f;

	move = Math::TransformNormal(move, worldtransformBase_.matWorld_);

	worldtransformBase_.translate = Math::Add(worldtransformBase_.translate, move);
}