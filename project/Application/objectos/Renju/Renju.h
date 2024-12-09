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
#include "RenjuBullet.h"

/**
 * @file Renju
 * @brief レンジャーキャラクターを制御するクラス
 */
class Renju : public BaseCharacter {

public: // メンバ関数
	~Renju() override;

	//レンジャーの初期化
	void Initialize(Animations* animation, std::string skillName) override;
	//毎フレームの更新
	void Update() override;
	//レンジャーにかかわるobjの描画
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

	//ため攻撃の初期化・更新
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
	WorldTransform& GetWorldTransformBow() { return worldTransformBow_; }
	// 弾リストの取得
	const std::list<RenjuBullet*>& GetBullets() const { return bullets_; }
	bool GetSpecial() {return special_;}
	bool GetHitBullet() { return hitBullet_; }
	bool GetSkill() { return skill_; }
	/*-----セッター-----*/
	void SetBarrierThreshold(float threshold) { barrierThreshold_ = threshold; }
	void SetTankPos(Vector3 pos) { tankPos_ = pos; }
	void SetHeal(float heal) {
		if (!isDead_) {
			hp_ += heal;
		}
	}


	void SetLight(DirectionLight directionLight)override { 
		BaseCharacter::SetLight(directionLight); 
		bulletModel_->DirectionalLightDraw(directionLight);
		bowModel_->DirectionalLightDraw(directionLight);
	}
	


private:
	
	// パーツ親子関係
	void Relationship() override;

private: // メンバ変数
	
	WorldTransform worldTransformBow_;
	WorldTransform worldTransformArrow_;

	std::unique_ptr<Model> bowModel_;
	std::unique_ptr<Model> bulletModel_;
	
	ParticleSystem* particle_;
	EmitterSphere emitter_{};
	GravityField filed_;
	int particleCount_;
	bool scaleFlag_;

	//ビヘイビアツリー
	BehaviorTree<Renju>* behaviorTree_;

	//攻撃フラグ
	bool hitBullet_ = false;

	//弾
	std::list<RenjuBullet*> bullets_;
	// 攻撃時間
	int fireTimer_ = 20;
	//子分を出してくる攻撃を止める
	bool special_ = false;
	int specialTimer_ = 60 * 8;

	//復活時間
	int revivalCount_ = 0;

	//ヒット確認
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;
	bool skill_ = false;

};