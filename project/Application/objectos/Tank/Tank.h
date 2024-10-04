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
#include <ParticleSystem.h>

/// <summary>
/// ゲームシーン
/// </summary>
class Tank : public Collider {

public: // メンバ関数
	~Tank();

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

	//スタン攻撃
	void UniqueInitialize();
	void UniqueUpdate();

	//死亡
	void DeadInitialize();
	void DeadUpdate();

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	// 敵を探す
	void searchTarget(Vector3 enemyPos);

	//敵の視野内にいるかどうか
	void IsVisibleToEnemy();
	//逃げる方向
	void RunAway();


	void Relationship();

	void SetLight(DirectionLight directionLight) { 
		animation_->DirectionalLightDraw(directionLight); 
		shield_->DirectionalLightDraw(directionLight);
	}

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);
	void OnCollision(const WorldTransform& worldTransform);
	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;
	Vector3 GetLocalPosition();
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	WorldTransform& GetWorldTransfromHp(int i) {
		if (i == 0) {
			return worldTransformHp_[0];
		}
		else if (i == 1) {
			return worldTransformHp_[1];
		}
		else if (i == 2) {
			return worldTransformHp_[2];
		}

		return worldTransformHp_[0];
	}
	WorldTransform& GetWorldTransformHead() { return worldTransformHead_; }
	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}
	float GetHp() { return hp_; }
	float GetMp() { return mp_; }
	void SetHeal(float heal) { hp_ += heal; }
	bool GetAttack() { return attack_; }
	bool GetStanAttack() { return stanAttack_; }

	//敵の情報の受け取り
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	bool IsDead() { return isDead_; }

	void Dissolve() {
		animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
		threshold_ += 0.004f;
		animation_->SetThreshold(threshold_);
	}

	void SetOperation(bool flag) { operation_ = flag; }

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformShield_;
	WorldTransform worldTransformHp_[3];
	WorldTransform worldTransformHead_;
	ViewProjection viewProjection_;
	std::unique_ptr<Model> model_;
	std::unique_ptr<Animations>animation_;
	// 目標の角度
	float destinationAngleY_ = 0.0f;
	//ビヘイビアツリー
	BehaviorTree<Tank>* behaviorTree_;
	CharacterState state_;
	CharacterState previousState_;
	std::unique_ptr<Model>shield_;

	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_{};

	// 速度
	Vector3 velocity_ = {};
	// 味方の押し出し処理
	Vector3 allyVelocity;

	// プレイヤー座標
	float minDistance_ = 5.0f;
	bool followPlayer_ = false;
	

	// 敵を探すフラグ
	bool searchTarget_ = false;

	// 攻撃時間
	int fireTimer_ = 40;

	uint32_t nockTime_;
	bool nockBack_;
	Vector3 enemyPos_;
	Vector3 playerPos_;

	bool attack_ = false;
	bool stanAttack_ = false;
	//攻撃ができるようになるまでの
	int coolTime = 60;

	//体力
	float hp_ = 100.0f;
	//マジックポイント
	float mp_ = 100.0f;
	//復活時間
	int revivalCount_;

	bool preHit_;
	bool isHit_;

	//プレイヤーと当たっているか
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;

	//死亡フラグ
	bool isDead_ = false;
	float threshold_;

	//敵情報
	Enemy* enemy_;
	float enemylength_;

	//作戦
	bool operation_;

	int jumpCount_;

	float kDegreeToRandian = 3.141592f / 180.0f;

	//敵の視野内
	//最小距離
	float enemyMinDistance_ = 2.0f;
	//最大距離
	float enemyMaxDistance_ = 45.0f;
	//角度範囲
	float angleRange_ = 35.0f * kDegreeToRandian;
	//敵の攻撃範囲ないかどうか
	bool isArea_ = false;
};
