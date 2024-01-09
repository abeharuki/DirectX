#pragma once
#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// ゲームシーン
/// </summary>
class Healer {

public: // メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Healer();

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

	// 攻撃
	void AttackInitialize();
	void AttackUpdata();

	// パーツ親子関係
	void Relationship();

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	// 敵を探す
	void searchTarget(Vector3 enemyPos);

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);


	Vector3 GetWorldPosition();
	Vector3 GetLocalPosition();
	WorldTransform& GetWorldTransform() { return worldTransformBase_; }
	WorldTransform& GetWorldTransformCane() { return worldTransformCane_; }
	WorldTransform& GetWorldTransformCollision() { return worldTransformCollision_; }

	bool IsAttack() { return workAttack_.isAttack; }

	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformCane_;
	WorldTransform worldTransformCollision_;
	ViewProjection viewProjection_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;

	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kJump, // ジャンプ
		kAttack, // 攻撃
		kDead, // 死亡
	};

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;


	// 攻撃用ワーク
	struct WorkAttack {
		uint32_t attackParameter_ = 0;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
		bool isAttack = false;
		bool isFinalAttack = false;
	};

	WorkAttack workAttack_;

	// 攻撃の時間
	const uint32_t behaviorAttackTime = 15;


	// 速度
	Vector3 velocity_ = {};
	// 味方の押し出し処理
	Vector3 allyVelocity;

	//プレイヤー座標
	float minDistance_ = 10.0f;
	bool followPlayer_ = false;

	
	// 敵を探すフラグ
	bool searchTarget_ = false;

	Vector3 enemyPos_;
};
