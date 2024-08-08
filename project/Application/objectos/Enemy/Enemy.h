#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Utilities/RandomGenerator.h"
#include "CollisionManager/Collider.h"
#include <Animation/Animation.h>

enum AnimationNumber {
	dashAttack,
	groundAttack,
	jumpAttack,
	nomal,
	nomalAttack,
	run,
	runUp,
	swing,
};


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

	void Draw(const ViewProjection& camera);

	// 移動
	void MoveInitialize();
	void MoveUpdata();
	//攻撃
	void AttackInitialize();
	void AttackUpdata();
	//死亡
	void DeadInitilize();
	void DeadUpdata();

	/*-----------------------攻撃レパートリー----------------------*/
	//通常攻撃
	void NomalAttackInitialize();
	void NomalAttackUpdata();
	//ダッシュ
	void DashAttackInitialize();
	void DashAttackUpdata();
	//投擲
	void ThrowingAttackInitialize();
	void ThrowingAttackUpdata();
	//たたきつけ
	void GroundAttackInitialize();
	void GroundAttackUpdata();

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	WorldTransform& GetWorldTransformRock() { return worldTransformRock_; }

	void SetLight(DirectionLight directionLight) { 
		animation_->DirectionalLightDraw(directionLight);
		impactModel_->DirectionalLightDraw(directionLight);
	
	}


	void SetPlayerPos(Vector3 pos) { playerPos_ = pos; };
	void SetHealerPos(Vector3 pos) { healerPos_ = pos; };
	void SetRenjuPos(Vector3 pos) { renjuPos_ = pos; };
	void SetTankPos(Vector3 pos) { tankPos_ = pos; };
	// パーツ親子関係
	void Relationship();

	bool IsBehaberAttack() {
		return  behaviorAttack_;
	}
	bool isAttack() { return isAttack_; };
	bool isClear() { return clear_; };
	void isDead(bool dead) {
		if (dead) {
			behavior_ = Behavior::kDead;
		};
	}

	//範囲の条件文
	bool isWithinRange(float value, float range ,float sub) {
		if (value >= range-sub && value <= range+sub) {
			return true;
		}
		else {
			return false;
		}
	}

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformRock_;
	WorldTransform worldTransformImpact_;
	std::unique_ptr<Animations>animation_;
	int animationNumber_;

	std::unique_ptr<Model> impactModel_;

	Vector3 playerPos_ = {};
	Vector3 healerPos_ = {};
	Vector3 renjuPos_ = {};
	Vector3 tankPos_ = {};

	bool isAttack_ = false;
	bool behaviorAttack_ = false;


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
		kNomal,//通常攻撃
		kDash, // ダッシュ攻撃
		kThrowing,//投擲攻撃
		kGround,//地面を殴る攻撃
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
	float threshold_ = 0.0f;

	//シェイク
	float shakeTimer_;
	float randX = 0;
	float randZ = 0;

};