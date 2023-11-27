#pragma once
#include "BaseCharacter.h"
#include "DirectXCommon.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"

class Enemy : public BaseCharacter {
public:
	/*Enemy();
	~Enemy();*/
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Vector3 pos);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewprojection, bool light) override;

	void Move();

	//// ワールド座標
	Vector3 GetWorldPosition();
	/// 中心座標
	Vector3 GetSenterPosition() const;

	const WorldTransform& GetWorldTransform() { return worldtransformBase_; }

	void OnCollision(const WorldTransform& worldTransform);
	bool IsDead() { return isDead_; }

private:
	WorldTransform worldtransformBase_;
	std::unique_ptr<Model> modelEnemy_;
	bool isHit_ = false;
	bool preHit_ = false;
	uint32_t hitCount = 0;
	bool isDead_;
	// 死亡アニメーションの速度
	Vector3 deathVelocity{0.0f, 0.0f, 0.0f};
	float speed = 1.0f;

	float a = 1.0f;
};