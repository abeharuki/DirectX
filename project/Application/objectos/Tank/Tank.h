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
class Tank : public BaseCharacter {

public: // メンバ関数
	~Tank() override;
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
	void SetHeal(float heal) { 
		if (!isDead_) {
			hp_ += heal;
		}
	}
	bool GetStanAttack() { return stanAttack_; }

	

	/*-----セッター-----*/
	void SetLight(DirectionLight directionLight) override{
		BaseCharacter::SetLight(directionLight);
		shield_->DirectionalLightDraw(directionLight);
	}

	

private:
	// パーツ親子関係
	void Relationship() override;

private: // メンバ変数
	WorldTransform worldTransformShield_;
	
	//ビヘイビアツリー
	BehaviorTree<Tank>* behaviorTree_;
	std::unique_ptr<Model>shield_;

	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_{};
	
	// 味方の押し出し処理
	Vector3 allyVelocity;

	uint32_t nockTime_;
	bool nockBack_;

	//攻撃フラグ
	bool stanAttack_ = false;
	// 攻撃時間
	int fireTimer_ = 40;
	//復活時間
	int revivalCount_ = 0;

	//プレイヤーと当たっているか
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;

};
