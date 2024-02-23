#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Utilities/RandomGenerator.h"
#include "CollisionManager/Collider.h"
/// <summary>
/// ゲームシーン
/// </summary>
class Enemy : public Collider {

public: // メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

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

	// 攻撃
	void AttackInitialize();
	void AttackUpdata();

	void DeadInitilize();
	void DeadUpdata();



	void DashAttackInitialize();
	void DashAttackUpdata();


	void ThrowingAttackInitialize();
	void ThrowingAttackUpdata();

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	WorldTransform& GetWorldTransformRock() { return worldTransformRock_; }

	void SetPlayerPos(Vector3 pos) { playerPos_ = pos; };
	void SetHealerPos(Vector3 pos) { healerPos_ = pos; };
	void SetRenjuPos(Vector3 pos) { renjuPos_ = pos; };
	void SetTankPos(Vector3 pos) { tankPos_ = pos; };
	// パーツ親子関係
	void Relationship();


	bool isAttack() { return isAttack_; };

	bool isClear() { return clear_; };

	void isDead(bool dead) {
		if (dead) {
			behavior_ = Behavior::kDead;
		};
	}

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformRock_;

	Vector3 playerPos_ = {};
	Vector3 healerPos_ = {};
	Vector3 renjuPos_ = {};
	Vector3 tankPos_ = {};

	bool isAttack_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;
	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kAttack, // 攻撃
		kDead, // 死亡
	};

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;


	enum class BehaviorAttack {
		kDash, // ダッシュ攻撃
		kThrowing,//投擲攻撃
	};

	BehaviorAttack attack_ = BehaviorAttack::kDash;
	// 次の振る舞いリクエスト
	std::optional<BehaviorAttack> attackRequest_ = std::nullopt;


	// 速度
	Vector3 velocity_ = {};
	//差分
	Vector3 sub;
	int time_;
	int num_;

	bool clear_;

	//シェイク
	float shakeTimer_;
	float randX = 0;
	float randZ = 0;

};