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

	//味方の回復
	void UniqueInitialize();
	void UniqueUpdate();

	//死亡
	void DeadInitialize();
	void DeadUpdate();

	// パーツ親子関係
	void Relationship();

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	// 敵を探す
	void searchTarget(Vector3 enemyPos);

	//敵の視野内にいるかどうか
	void IsVisibleToEnemy();
	//逃げる方向
	void RunAway();

	void SetLight(DirectionLight directionLight) { 
		animation_->DirectionalLightDraw(directionLight);
		DirectionLight light;
		light = directionLight;
		light.direction *= -1.0f;
		for (int i = 0; i < 4; ++i) {
			magicCircle_[i]->DirectionalLightDraw(light);
		}
		
	}

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
	float GetHp() { return hp_; }
	float GetMp() { return mp_; }
	bool GetHeal() { return heal_; }
	bool GetAllHeal() { return allHeal_; }
	bool GetOneHeal() { return oneHeal_; }
	float GetHealAmount() { return healAmount_; }
	void SetViewProjection(const ViewProjection& viewProjection) {viewProjection_ = viewProjection;}
	//座標の受け取り
	void SetPos(Vector3 playerPos, Vector3 renjuPos, Vector3 tankPos) {
		pos[0] = playerPos;
		pos[1] = renjuPos;
		pos[2] = tankPos;
	}
	//敵の情報の受け取り
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }
	//味方キャラのHpの受け取り
	void SetHp(Vector3 hp) { 
		playerHp_ = hp.x;
		renjuHp_ = hp.y;
		tankHp_ = hp.z;
	}
	void SetHeal(float heal) { hp_ += heal; }
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
	WorldTransform worldTransformMagicCircle_[4];
	ViewProjection viewProjection_;

	//ダメージ表示
	WorldTransform worldTransformNum_;
	std::unique_ptr<Model> damageModel_;
	float alpha_;
	Vector3 numMove_;

	std::unique_ptr<Animations>animation_;
	int animationNumber_;
	enum AnimationNumber {
		//animeAttack,//攻撃
		jump,//ジャンプ
		run,//移動
		standby,//待機
	};

	float flameTime_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;
	//魔法陣
	std::unique_ptr<Model> magicCircle_[4];
	float t_[4];
	Vector3 pos[3];

	//パーティクル
	std::vector<std::unique_ptr<ParticleSystem>> particle_;
	std::vector<EmitterSphere> emitter_{};

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
	float minDistance_ = 8.0f;
	bool followPlayer_;
	

	// 敵を探すフラグ
	bool searchTarget_ = false;

	Vector3 enemyPos_;

	uint32_t nockTime_;
	bool nockBack_;

	//体力
	float hp_ = 100.0f;
	//マジックポイント
	float mp_ = 100.0f;
	//回復フラグ
	bool allHeal_;
	bool oneHeal_;
	bool heal_;
	//回復量
	float healAmount_;
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

	int jumpCount_;
	float enemylength_;

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

	//味方のHp
	float playerHp_;
	float renjuHp_;
	float tankHp_;

};