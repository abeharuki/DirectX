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

/// <summary>
/// ゲームシーン
/// </summary>
class Renju : public BaseCharacter {

public: // メンバ関数
	~Renju() override;
	
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

	//ため攻撃
	void UniqueInitialize() override;
	void UniqueUpdate() override;

	//死亡
	void DeadInitialize() override;
	void DeadUpdate() override;

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;

	/*-----ゲッター-----*/
	WorldTransform& GetWorldTransformBow() { return worldTransformBow_; }
	// 弾リストの取得
	const std::list<RenjuBullet*>& GetBullets() const { return bullets_; }
	bool GetHitBullet() { return hitBullet_; }

	/*-----セッター-----*/
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
	
	// 味方の押し出し処理
	Vector3 allyVelocity;

	//攻撃フラグ
	bool hitBullet_ = false;

	//弾
	std::list<RenjuBullet*> bullets_;
	// 攻撃時間
	int fireTimer_ = 20;
	

	//復活時間
	int revivalCount_ = 0;

	//ヒット確認
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;
};