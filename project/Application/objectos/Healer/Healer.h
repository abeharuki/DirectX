#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <CollisionManager/Collider.h>
#include <Animation/Animation.h>
#include "../../BehaviorTree/BehaviorTree.h"
#include "Enemy/Enemy.h"

/// <summary>
/// ゲームシーン
/// </summary>
class Healer : public Collider {

public: // メンバ関数
	~Healer();
	void SetState(CharacterState newState) {
		state_ = newState;
	}

	CharacterState GetState() const {
		return state_;
	}

	
	void Initialize();

	void Update();

	void Draw(const ViewProjection& camera);

	// 移動
	void MoveInitialize();
	void MoveUpdate();

	// ジャンプ
	void JumpInitialize();
	void JumpUpdate();

	// ノックバック
	void knockInitialize();
	void knockUpdate();

	// 攻撃
	void AttackInitialize();
	void AttackUpdate();

	//死亡
	void DeadInitialize();
	void DeadUpdate();

	// パーツ親子関係
	void Relationship();

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	// 敵を探す
	void searchTarget(Vector3 enemyPos);

	void Billboard();

	void SetLight(DirectionLight directionLight) { animation_->DirectionalLightDraw(directionLight); }

	void SetOperation(bool flag) { operation_ = flag; }

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);
	void OnCollision(const WorldTransform& worldTransform);
	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;
	Vector3 GetLocalPosition();
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	WorldTransform& GetWorldTransformHead() { return worldTransformHead_; }
	WorldTransform& GetWorldTransfromHp(int i) { 
		if (i == 0) {
			return worldTransformHp_[0];
		}
		else if(i == 1){
			return worldTransformHp_[1];
		}
		else if (i == 2) {
			return worldTransformHp_[2];
		}

		return worldTransformHp_[0];
	}
	WorldTransform& GetWorldTransformCane() { return worldTransformCane_; }
	WorldTransform& GetWorldTransformCollision() { return worldTransformCollision_; }

	bool IsAttack() { return workAttack_.isAttack; }
	int GetHitCount() { return hitCount_; }
	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

	//敵の情報の受け取り
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }
	
	bool IsDead() { return isDead_; }

	void Dissolve() {
		animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
		threshold_ += 0.004f;
		animation_->SetThreshold(threshold_);
	}

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHp_[3];
	WorldTransform worldTransformCane_;
	WorldTransform worldTransformCollision_;
	ViewProjection viewProjection_;

	std::unique_ptr<Animations>animation_;
	// 目標の角度
	float destinationAngleY_ = 0.0f;

	//ビヘイビアツリー
	BehaviorTree<Healer>* behaviorTree_;
	CharacterState state_;
	CharacterState previousState_;

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
	//攻撃ができるようになるまでの
	int coolTime = 60;

	// 速度
	Vector3 velocity_ = {};
	// 味方の押し出し処理
	Vector3 allyVelocity;

	//プレイヤー座標
	float minDistance_ = 2.5f;
	bool followPlayer_;
	

	// 敵を探すフラグ
	bool searchTarget_ = false;

	Vector3 enemyPos_;

	uint32_t nockTime_;
	bool nockBack_;

	//体力
	int hitCount_ = 3;
	//復活時間
	int revivalCount_ = 0;

	bool preHit_;
	bool isHit_;
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;

	//死亡フラグ
	bool isDead_ = false;
	float threshold_;

	//敵情報
	Enemy* enemy_;
	
	//作戦
	bool operation_;
};