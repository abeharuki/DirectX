#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <CollisionManager/Collider.h>

/// <summary>
/// ゲームシーン
/// </summary>
class Tank : public Collider {

public: // メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Tank();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	// 移動
	void MoveInitialize();
	void MoveUpdata();

	// ジャンプ
	void JumpInitialize();
	void JumpUpdata();

	// ノックバック
	void knockInitialize();
	void knockUpdata();

	// 攻撃
	void AttackInitialize();
	void AttackUpdata();

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	// 敵を探す
	void searchTarget(Vector3 enemyPos);

	void Relationship();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);
	void OnCollision(const WorldTransform& worldTransform);
	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;
	Vector3 GetLocalPosition();
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	WorldTransform& GetWorldTransformHead() { return worldTransformHead_; }
	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}
	bool IsHit() { return hitCount_; }

	bool GetAttack() { return attack_; }

	void SetEnemyAttack(bool attack) { isEnemyAttack_ = attack; }

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	ViewProjection viewProjection_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;

	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kJump, // ジャンプ
		knock,   // ノックバック
		kAttack, // 攻撃
		kDead, // 死亡
	};

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	// 速度
	Vector3 velocity_ = {};
	// 味方の押し出し処理
	Vector3 allyVelocity;

	// プレイヤー座標
	float minDistance_ = 10.0f;
	bool followPlayer_ = false;

	// 敵を探すフラグ
	bool searchTarget_ = false;

	// 攻撃時間
	int fireTimer_ = 40;

	uint32_t nockTime_;
	Vector3 enemyPos_;

	bool attack_ = false;

	//体力
	bool hitCount_;

	bool preHit_;
	bool isHit_;

	//敵の攻撃フラグ
	bool isEnemyAttack_;

};
