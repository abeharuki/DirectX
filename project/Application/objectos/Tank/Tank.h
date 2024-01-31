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
class Tank {

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

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);
	void OnCollision(const WorldTransform& worldTransform);


	Vector3 GetWorldPosition();
	Vector3 GetLocalPosition();
	WorldTransform& GetWorldTransform() { return worldTransformBase_; }

	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

	bool GetAttack() { return attack_; }

private: // メンバ変数
	WorldTransform worldTransformBase_;
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

	//味方の判定
	bool preHit_;
	bool isHit_;
};
