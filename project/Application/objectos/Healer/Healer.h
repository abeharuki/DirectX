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


/**
 * @file Healer
 * @brief ヒーラーキャラクターを制御するクラス
 */
class Healer : public BaseCharacter {

public: // メンバ関数
	~Healer() override;	
	//ヒーラーの初期化
	void Initialize(Animations* animation, std::string skillName) override;
	//毎フレームの更新
	void Update() override;
	//ヒーラーにかかわるobjの描画
	void Draw(const ViewProjection& camera) override;
	//深度値がないものの描画
	void NoDepthDraw(const ViewProjection& camera) override;

	// 移動の初期化・更新
	void MoveInitialize() override;
	void MoveUpdate() override;

	// ジャンプの初期化・更新
	void JumpInitialize() override;
	void JumpUpdate() override;

	// 攻撃の初期化・更新
	void AttackInitialize() override;
	void AttackUpdate() override;

	//味方の回復の初期化・更新
	void UniqueInitialize() override;
	void UniqueUpdate() override;

	//ブレス攻撃の回避
	void BreathInitialize() override;
	void BreathUpdate() override;

	//味方AIを守る動きの初期化・更新
	void ProtectInitialize() override;
	void ProtectUpdate() override;

	//死亡の初期化・更新
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
	float GetPlayerHp() { return playerHp_; }
	float GetRenjuHp() { return renjuHp_; }
	float GetTankHp() { return tankHp_; }

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
	void SetBarrierThreshold(float threshold) { barrierThreshold_ = threshold; }
	//味方AI座標の受け取り
	void SetPos(Vector3 renjuPos, Vector3 tankPos) {
		renjuPos_ = renjuPos;
		tankPos_ = tankPos;
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
	//回復数値
	std::unique_ptr<Model> healModel_[4];
	float t_[4];//ディゾルブ
	float healAlph_[4];
	Vector3 healNumMove_[4];

	//パーティクル
	std::vector<ParticleSystem*> particle_;
	std::vector<EmitterSphere> emitter_{};

	//ビヘイビアツリー
	BehaviorTree<Healer>* behaviorTree_;


	// 攻撃の時間
	uint32_t attackParameter_ = 0;

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