#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class RenjuBullet {
public:

	void Initialize(
	    Model* model, Vector3& position, const Vector3& scale, Vector3& rotation,
	    const Vector3& velocity);


	void Update();

	
	void Draw(const ViewProjection& viewprojection);

	bool IsDead() const { return isDead_; }

	// ワールド座標を取得
	Vector3 GetWorldPosition();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	// ギミック初期化
	void InitializeFloatingGimmick();

	// ギミックの更新
	void UpdateFloatingGimmick();

private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Utility* utility_ = nullptr;

	// 速度
	Vector3 velocity_;

	// 寿命
	static const int32_t kLifeTime = 60 * 5;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;

	// 浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;
	// 浮遊移動サイクル
	float cycle = 0;
	// 浮遊の振幅
	float amplitude = 0.0f;
	float amplitudeArm = 0.4f;
	// 円周率
	float Pi = 0.0f;
};