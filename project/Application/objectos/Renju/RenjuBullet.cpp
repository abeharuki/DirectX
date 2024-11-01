#include "RenjuBullet.h"
#include <cassert>

Vector3 RenjuBullet::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}



/// <summary>
/// 初期化
/// </summary>
void RenjuBullet::Initialize(
	Model* model, Vector3& position, const Vector3& scale, Vector3& rotation,
	const Vector3& velocity) {

	assert(model);

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translate = position;
	worldTransform_.scale = scale;
	worldTransform_.rotate = rotation;
	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void RenjuBullet::Update() {

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	worldTransform_.translate = worldTransform_.translate + velocity_;

	worldTransform_.UpdateMatrix();
};

/// <summary>
/// 描画
/// </summary>
void RenjuBullet::Draw(const ViewProjection& viewprojection) {
	model_->Draw(worldTransform_, viewprojection, false);
};

void RenjuBullet::OnCollision() {
	isDead_ = true;
}