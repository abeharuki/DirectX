#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "../../BehaviorTree/BehaviorTree.h"
#include "Enemy/Enemy.h"
#include <ParticleSystem.h>
#include "base/BaseCharacter.h"


/// <summary>
/// ゲームシーン
/// </summary>
class Healer : public BaseCharacter {

public: // メンバ関数
	~Healer() override;	
	void Initialize(Animations* animation, std::string skillName) override;

	void Update() override;

	void Draw(const ViewProjection& camera) override;
	void NoDepthDraw(const ViewProjection& camera) override;

	// 移動
	void MoveInitialize() override;
	void MoveUpdate() override;

	// ジャンプ
	void JumpInitialize() override;
	void JumpUpdate() override;

	// 攻撃
	void AttackInitialize() override;
	void AttackUpdate() override;

	//味方の回復
	void UniqueInitialize() override;
	void UniqueUpdate() override;

	//死亡
	void DeadInitialize() override;
	void DeadUpdate() override;

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;


	/*-----ゲッター-----*/
	WorldTransform& GetWorldTransformCane() { return worldTransformCane_; }
	bool GetHeal() { return heal_; }
	bool GetAllHeal() { return allHeal_; }
	bool GetOneHeal() { return oneHeal_; }
	float GetHealAmount() { return healAmount_; }

	/*-----セッター-----*/
	void SetLight(DirectionLight directionLight) override{
		BaseCharacter::SetLight(directionLight);
		DirectionLight light;
		light = directionLight;
		light.direction *= -1.0f;
		for (int i = 0; i < 4; ++i) {
			magicCircle_[i]->DirectionalLightDraw(light);
		}

	}

	/*--------このクラス特有の関数-------*/
	//座標の受け取り
	void SetPos(Vector3 renjuPos, Vector3 tankPos) {
		pos[0] = playerPos_;
		pos[1] = renjuPos;
		pos[2] = tankPos;
	}
	//味方キャラのHpの受け取り
	void SetHp(Vector3 hp) { 
		playerHp_ = hp.x;
		renjuHp_ = hp.y;
		tankHp_ = hp.z;
	}
	void SetHeal(float heal) {
		if (!isDead_) {
			hp_ += heal;
		}
		
	}
	
private:
	// パーツ親子関係
	void Relationship() override;

private: // メンバ変数
	WorldTransform worldTransformCane_;
	WorldTransform worldTransformMagicCircle_[4];
	WorldTransform worldTransformHeal_[4];
	enum Character {
		healer,
		player,
		renju,
		tank,
	};
	//魔法陣
	std::unique_ptr<Model> magicCircle_[4];
	std::unique_ptr<Model> healModel_[4];
	float t_[4];//ディゾルブ
	Vector3 pos[3];
	float healAlph_[4];
	Vector3 healNumMove_[4];

	//パーティクル
	std::vector<ParticleSystem*> particle_;
	std::vector<EmitterSphere> emitter_{};

	//ビヘイビアツリー
	BehaviorTree<Healer>* behaviorTree_;

	// 攻撃用ワーク
	struct WorkAttack {
		uint32_t attackParameter_ = 0;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
		bool isFinalAttack = false;
	};

	WorkAttack workAttack_;

	// 攻撃の時間
	const uint32_t behaviorAttackTime = 15;

	// 味方の押し出し処理
	Vector3 allyVelocity;

	//回復フラグ
	bool allHeal_;//全体回復
	bool oneHeal_;//個人回復
	bool heal_;//ヒールする瞬間
	bool healAnimation_;//ヒールの演出

	//回復量
	float healAmount_;
	//復活時間
	int revivalCount_ = 0;
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;

	//味方のHp
	float playerHp_;
	float renjuHp_;
	float tankHp_;

};