#pragma once
#include <CollisionManager/Collider.h>
#include <Model.h>

namespace RenjuBulletConstants {
	const float kShockAlphaDecrement = 0.1f; // 衝撃波のアルファ減少量
	const float kShockwaveAlphaThresholdMid = 0.5f; // 中間アルファ閾値
	const float kShockwaveAlphaThresholdLow = 0.1f; // 低アルファ閾値
	const float kShockwaveThresholdMax = 1.0f; // 衝撃波の最大しきい値
	
}

/**
 * @file RenjuManager
 * @brief レンジャーが打つ矢を制御するクラス
 */
class RenjuBullet : public Collider {
public:

	//弾の初期化
	void Initialize(
		Model* model, Vector3& position, const Vector3& scale, const Vector3& rotation,
		const Vector3& velocity,bool skill);

	//弾の更新
	void Update();

	//弾の描画
	void Draw(const ViewProjection& viewprojection);
	
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;

	/*-----ゲッター-----*/
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransform_; }

	bool IsDead() const { return isDead_; }
	bool IsDraw() const { return Draw_; }
private:
	WorldTransform worldTransform_;
	WorldTransform worldTransformShockwave_[3];
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Utility* utility_ = nullptr;

	std::unique_ptr<Model> shockwave_[3];

	// 速度
	Vector3 velocity_;

	// 寿命
	static const int32_t kLifeTime = 60;
	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
	//スキル攻撃かどうか
	bool skill_;
	//描画フラグ
	bool Draw_;


	//衝撃波の状態
	struct ShockWave {
		Vector3 scaleVelocity_;
		float shockVelocity_;
		float alpha_;
		float threshold_;
		bool isActive;
	};

	ShockWave shockData_[3];
};
