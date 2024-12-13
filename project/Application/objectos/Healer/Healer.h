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

//ヒーラークラスの定数
namespace HealerConstants {
	//各worldTransformの初期化
	const Vector3 kBaseTranslate = { 6.0f,0.0f,0.0f };//キャラクターの基本位置
	const Vector3 kCaneTranslate = { -0.03f, 0.04f, 0.1f };//杖の初期位置
	const Vector3 kCaneRotate = { 1.3f, 0.f, 0.f };//杖の初期回転角度
	const Vector3 kCaneScale = { 1.f, 1.f, 1.f }; //杖の初期サイズ
	const Vector3 kMagicCircleTranslate = { 0.0f,0.11f,0.0f };//魔法陣の初期座標
	const Vector3 kMagicCircleRotate = { -1.571f,0.0f,0.0f };//魔法陣の初期角度
	const Vector3 kMagicCircleScale = { 2.0f,2.0f,2.0f };//魔法陣の初期サイズ
	const Vector3 kHealScale = { 0.5f,0.5f,0.5f };//回復数値描画の初期サイズ

	

	const int kEmitterCount = 5;//エミッターのMax数
	const float kHealAlphaDecrement = 0.08f;//蛭海フック数値モデルのアルファ値の変化

	const int kCoolTimeReset = 60;//クールタイム初期化値

	// HP関連の閾値
	const int kSingleHealHpThreshold = 30;  //単体回復を行うHPの閾値
	const int kAllHealHpThreshold = 50;    //全体回復を行うHPの閾値
	const int kSingleHealAmount = 40;         //単体回復量
	const int kAllHealAmount = 20;            //全体回復量
	const int kHealThreshold = 30;            //回復対象のHP閾値
	const Vector3 kHealAnimationScale = { 0.5f,0.5f,0.5f };//ヒール中のスケール
	const float kMagicCircleScaleIncrease = 0.5f;//クールタイムによるスケール増加
	const float kMagicCircleScaleDecrease = 1.0f;//クールタイムによるスケール減少
	const float kHealPositionYOffset = 2.0f;//回復位置のY方向オフセット

	// MP関連の閾値
	const int kSingleHealMpCost = 10;//単体回復の消費MP
	const int kAllHealMpCost = 20;//全体回復の消費MP

	// 敵との距離関連
	const float kMinDistanceMultiplier = 2.0f;//最小距離倍率

	// 回復エフェクトの閾値
	const float kThresholdDecrease = 0.02f;//ヒール中の閾値減少量
	const float kMagicThreshold = 0.8f;//魔法陣の閾値
	const float kHealNumThreshold = 2.0f;//回復数値の閾値

	// クールタイムの閾値
	const int kCoolTimeThreshold = 20;//スキル使用可能なクールタイム閾値
	
	// 攻撃関連
	const int kAttackStartFrames = 25;          // 攻撃開始までのフレーム数
	const int kAttackEndFrames = 60;            // 攻撃終了までのフレーム数
	const int kAttackCoolTime = 60;             // 攻撃後のクールタイム


	
	
}


/**
 * @file Healer
 * @brief ヒーラーキャラクターを制御するクラス
 */
class Healer : public AllyAICharacter {

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
		AllyAICharacter::SetLight(directionLight);
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
		kHealer,
		kPlayer,
		kRenju,
		kTank,
		kMaxCharacterNum,
	};
	//魔法陣
	std::unique_ptr<Model> magicCircle_[4];
	//回復数値
	std::unique_ptr<Model> healModel_[4];
	float threshold_[4];//ディゾルブ
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