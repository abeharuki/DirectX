#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "RenjuBullet.h"
#include <CollisionManager/Collider.h>

/// <summary>
/// ゲームシーン
/// </summary>
class Renju : public Collider {

public: // メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Renju();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void Draw(const ViewProjection& view);

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

	//敵を探す
	void searchTarget(Vector3 enemyPos);

	// パーツ親子関係
	void Relationship();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);
	void OnCollision(const WorldTransform& worldTransform);
	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;
	Vector3 GetLocalPosition();
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	WorldTransform& GetWorldTransformHead() { return worldTransformHead_; }
	// 弾リストの取得
	const std::list<RenjuBullet*>& GetBullets() const { return bullets_; }
	bool IsHit() { return hitCount_; }

	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

	void SetEnemyAttack(bool attack) { isEnemyAttack_ = attack; }

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	ViewProjection viewProjection_;

	std::unique_ptr<Model> bulletModel_;


	// 目標の角度
	float destinationAngleY_ = 0.0f;

	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kJump, // ジャンプ
		knock,   // ノックバック
		kAttack,//攻撃
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

	//敵を探すフラグ
	bool searchTarget_ = false;

	//弾
	std::list<RenjuBullet*> bullets_;
	// 攻撃時間
	int fireTimer_ = 20;

	Vector3 enemyPos_;
	uint32_t nockTime_;

	//体力
	bool hitCount_;

	bool preHit_;
	bool isHit_;

	//敵の攻撃フラグ
	bool isEnemyAttack_;
};
