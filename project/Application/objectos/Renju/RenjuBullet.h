#pragma once
#include <CollisionManager/Collider.h>
#include <Model.h>

/**
 * @file RenjuManager
 * @brief レンジャーが打つ矢を制御するクラス
 */
class RenjuBullet : public Collider {
public:

	void Initialize(
		Model* model, Vector3& position, const Vector3& scale, const Vector3& rotation,
		const Vector3& velocity,bool skill);


	void Update();

	void Draw(const ViewProjection& viewprojection);

	bool IsDead() const { return isDead_; }
	bool IsDraw() const { return Draw_; }
	
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;

	/*-----ゲッター-----*/
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransform_; }

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
