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

// ギミック初期化
void RenjuBullet::InitializeFloatingGimmick() {
	floatingParameter_ = 0.0f;

	// 浮遊移動サイクル
	cycle = 60;
	Pi = 3.1415f;
	// 浮遊の振幅
	amplitude = 0.2f;
	amplitudeArm = 0.4f;
}

// ギミック
void RenjuBullet::UpdateFloatingGimmick() {
	// 1フレームでのパラメータ加算値
	const float steppe = 2.0f * Pi / cycle;

	// パラメータを1分加算
	floatingParameter_ += steppe;
	// 2πを超えたら0に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * Pi);

	// 浮遊を座標に反映
	worldTransform_.translate.y = std::sin(floatingParameter_) * amplitude;
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

	worldTransform_.translate = worldTransform_.translate +velocity_;

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