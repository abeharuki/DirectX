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
#include "base/AllyAICharacter.h"
#include "RenjuBullet.h"

//レンジャークラスの定数
namespace RenjuConstants {
	// ワールドトランスフォームの初期設定
	const Vector3 kBowRotate = { 1.9f, 0.0f, -1.5f }; // 弓の初期回転角度
	const Vector3 kBaseTranslat = { -3.0f, 0.0f, 0.0f }; // ベースの初期位置
	const Vector3 kAttackArrowRotate = { -0.3f,0.0f,1.3f };//弓の角度

	// クールタイム
	const int kCoolTime = 60; //クールタイムの初期値

	// 距離関連
	const int kTargetDistance = 2; // 敵との距離感覚
	const float kMinDistance = 10.0f; // 最小距離
	const float kMinDistanceMultiplier = 1.5f; // 最小距離倍率
	const float kMinDistanceProtectMultiplier = 3.0f; // Protect状態の距離倍率

	//スキル関連
	const int kUniqueInitFireTimer = 60; // 初期FireTimer
	const float kUniqueInitFlameTime = 60.0f; // 初期FlameTime
	const int kSpecialTimerDuration = 60 * 7; // SpecialTimerの初期値
	const int kFireTimerCharge = 48; // チャージ中のFireTimer
	const float kCoolTimeSkill = 10.0f; // スキルを打つ時のクールタイム


	// MP関連
	const int kSkillMpCost = 10; // スキルの消費MP
	const int kProtectMpCost = 20; // Protectの消費MP

	// 攻撃関連
	const int kAttackInitFireTimer = 20; // 初期FireTimer
	const float kAttackInitFlameTime = 60.0; // 初期FlameTime
	

	// 弾関連
	const Vector3 kBulletScale = { 1.5f, 1.5f, 1.5f }; // 弾のスケール
	const float kBulletRotationX = 1.2f; // 弾のX軸回転
	const float kBulletYOffset = 7.0f; // 弾のYオフセットfloat

	// プロテクト関連
	const int kProtectCoolTime = 30; // クールタイム
	const float kRandomPositionOffsetX = 0.5f; // ランダム位置のXオフセット
	const int kSpecialTimerIncrement = 30; // 特殊タイマーの増加値(子分にぶつかった時に増える)

	// プレイヤーのダメージ関連
	const float kDamageAlphaInitValue = 2.0f;  // プレイヤーが食らったときダメージ表示の初期アルファ値（透明度）
	const float kEnemyDamageHenchman = 10.0f;  // チャージ中に子分にぶつかった時のダメージ値
}

/**
 * @file Renju
 * @brief レンジャーキャラクターを制御するクラス
 */
class Renju : public AllyAICharacter {

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
		AllyAICharacter::SetLight(directionLight); 
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
	int fireTimer_ = 0;
	//子分を出してくる攻撃を止める
	bool special_ = false;
	int specialTimer_ = 0;

	//復活時間
	int revivalCount_ = 0;

	//ヒット確認
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;
	bool skill_ = false;

};