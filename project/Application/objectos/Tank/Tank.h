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
 * @file Tank
 * @brief タンクキャラクターを制御するクラス
 */
class Tank : public BaseCharacter {

public: // メンバ関数
	~Tank() override;

	//タンクの初期化
	void Initialize(Animations* animation, std::string skillName) override;
	//毎フレームの更新
	void Update() override;

	//タンクにかかわるobjの描画
	void Draw(const ViewProjection& camera) override;
	//深度値がないものの描画
	void NoDepthDraw(const ViewProjection& camera) override;
	//バリアの描画
	void BarrierDraw(const  ViewProjection& camera);

	// 移動の初期化・更新
	void MoveInitialize() override;
	void MoveUpdate() override;

	// ジャンプの初期化・更新
	void JumpInitialize() override;
	void JumpUpdate() override;

	// 攻撃の初期化・更新
	void AttackInitialize() override;
	void AttackUpdate() override;

	//barrierの展開の初期化・更新
	void UniqueInitialize() override;
	void UniqueUpdate() override;

	//ブレス攻撃の回避の初期化・更新
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
	Vector3 GetBarrierWorldPos();
	bool GetStanAttack() { return stanAttack_; }
	float GetBarrierThreshold() { return barrierThreshold_; }
	

	/*-----セッター-----*/
	void SetHeal(float heal) {
		if (!isDead_) {
			hp_ += heal;
		}
	}
	void SetLight(DirectionLight directionLight) override{
		BaseCharacter::SetLight(directionLight);
		//shield_->DirectionalLightDraw(directionLight);
	}

	//味方AIのpos
	void SetPos(Vector3 renjuPos) {
		renjuPos_ = renjuPos;
	}

private:
	// パーツ親子関係
	void Relationship() override;

private: // メンバ変数
	WorldTransform worldTransformBarrier_;
	
	//ビヘイビアツリー
	BehaviorTree<Tank>* behaviorTree_;
	std::unique_ptr<Model>barrierModel_;
	
	ParticleSystem* particle_;
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
