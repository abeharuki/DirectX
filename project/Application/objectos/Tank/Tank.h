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

//タンククラスの定数
namespace TankConstants {
	// バリアの設定
	const float kBarrierThreshold = 1.0f; // バリアのしきい値
	const Vector3 kBarrierScale = { 7.f,7.f,7.f };; // バリアのスケール
	const float kBarrierThresholdIncrement = 0.03f; // バリアを閉じる時のスピード
	const float kBarrierThresholdDecrement = 0.01f;//バリアを展開するときのスピード
	const Vector4 kBarrierColor = { 0.f,1.0f,1.0f,0.4f };//バリアの色
	
	// 距離関連
	const float kMinDistance = 6.0f;  // 最小距離
	const int kTargetDistance = 2; // 敵との距離感覚
	const int kTargetBareathDistance = 3; // ブレス攻撃時の敵との距離感覚

	// 攻撃関連
	const int kFireTimerInit = 40;     // 攻撃初期化時のfireTimerの値
	const int kFireTimerThreshold1 = 10;  // fireTimerが10以上のときの閾値
	const int kFireTimerThreshold2 = 5;   // fireTimerが5以下10以上のときの閾値
	const int kCoolTimeInit = 60;      // クールタイム初期化値
	
	
	// mp関連
	const int kBreathMpCost = 20;  // ブレス技の消費MP
}


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

	//攻撃フラグ
	bool stanAttack_ = false;
	// 攻撃時間
	int fireTimer_ = 0;
	//復活時間
	int revivalCount_ = 0;

	//プレイヤーと当たっているか
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;

};
