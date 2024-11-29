#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "Utilities/RandomGenerator.h"
#include "CollisionManager/Collider.h"
#include <Animation/Animation.h>
#include <CollisionManager/ColliderManager.h>
#include "Editor/editor.h"
#include <AnimationManager.h>
#include <ParticleSystem.h>
#include "EnemyHenchman.h"

enum AnimationNumber {
	dashAttack,//ダッシュ攻撃
	death,//死亡
	groundAttack,//地面を殴る攻撃
	nomalAttack,//通常攻撃
	run,//走る
	runUp,//ダッシュ攻撃の走り出し
	standby,//通常状態
	swing,//投擲攻撃
	threat,//威嚇
};

// 振る舞い
enum class Behavior {
	kRoot, // 通常状態
	kAttack, // 攻撃
	kStan,//スタン
	kDead, // 死亡
};

enum class BehaviorAttack {
	kNomal,//通常攻撃
	kDash, // ダッシュ攻撃
	kThrowing,//投擲攻撃
	kGround,//地面を殴る攻撃
	kSpecial,//必殺技(体力が半分以下になった時に発動)
	kSpecial2,//必殺技(体力が半分以下になった時に発動)
};

class Enemy : public Collider {

public: // メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	//描画
	void Draw(const ViewProjection& camera);
	//デプスがないオブジェクトの描画
	void NotDepthDraw(const ViewProjection& camera);

	//当たりは判定
	void OnCollision(Collider* collider) override;

	//強制的にスタン状態にする
	void StanBehavior();

	//必殺技を打てる回数を増やす
	void AddSpecialCount();

	/*-----ゲッター-----*/
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	BehaviorAttack GetBehaviorAttack() { return attack_; }
	Behavior GetBehavior() { return behavior_; }
	Behavior GetBehaviorRequest() {
		if (behaviorRequest_) {
			return behaviorRequest_.value();
		}
		return Behavior::kRoot;
	}

	bool IsBehaviorRequest(){
		if (behaviorRequest_) {
			return true;
		}
		return false;
	}
	Collider* GetCollider(int i) { return colliderManager_[i].get(); }
	Collider* GetRockCollider() { return colliderRockManager_.get(); }
	// 敵リストの取得
	const std::list<EnemyHenchman*>& GetEnemys() const { return henchmans_; }
	WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	WorldTransform& GetWorldTransformRock() { return worldTransformRock_; }
	WorldTransform& GetWorldTransformArea() { return worldTransformArea_; }

	bool GetAimPlayer() { return aimPlayer_; }
	bool GetAimHealer() { return aimHealer_; }
	bool GetAimRenju() { return aimRenju_; }
	bool GetAimTank() { return aimTank_; }
	bool GetBattle() { return battleStart_; }

	float GetImpactSize() { return worldTransformImpact_.scale.z; }

	bool isAttack() { return isAttack_; };
	bool isClear() { return clear_; };
	bool IsAreaDraw() { return areaDraw_; }
	bool IsBehaberAttack() {return  behaviorAttack_;}

	/*-----セッター-----*/
	void SetHP(float hp) { hp_ = hp; }
	void SetLight(DirectionLight directionLight) { 
		animation_->DirectionalLightDraw(directionLight);
		impactModel_->DirectionalLightDraw(directionLight);
		for (int i = 0; i < 3; ++i) {
			sterModel_[i]->DirectionalLightDraw(directionLight);
		}
	}

	//アニメーションナンバー、フレームタイム、ループ
	void SetAnimationNumber(int num, float flame,bool flag) { animationNumber_ = num; animation_->SetFlameTimer(flame); animation_->SetLoop(flag); }
	void SetPlayerPos(Vector3 pos) { playerPos_ = pos; };
	void SetHealerPos(Vector3 pos) { healerPos_ = pos; };
	void SetRenjuPos(Vector3 pos) { renjuPos_ = pos; };
	void SetTankPos(Vector3 pos) { tankPos_ = pos; };
	void SetTankRotation(Vector3 rotation) { tankRotation_ = rotation; };
	void SethmansRenjuPos(Vector3 pos) { hmansRenjuPos_ = pos; }
	void SetIsDeadPlayer(bool flag) { isDeadPlayer_ = flag; }
	void SetIsDeadHealer(bool flag) { isDeadHealer_ = flag; }
	void SetIsDeadRenju(bool flag) { isDeadRenju_ = flag; }
	void SetIsDeadTank(bool flag) { isDeadTank_ = flag; }
	void SetBattleStart(bool flag) { battleStart_ = flag;}

	void SetDead(bool dead) {
		if (dead) {
			behaviorRequest_ = Behavior::kDead;
		};
	}

	
private:
	// 移動
	void MoveInitialize();
	void MoveUpdata();
	//攻撃
	void AttackInitialize();
	void AttackUpdata();
	//死亡
	void DeadInitilize();
	void DeadUpdata();

	/*-----------------------攻撃レパートリー----------------------*/
	//通常攻撃
	void NomalAttackInitialize();
	void NomalAttackUpdata();
	//ダッシュ
	void DashAttackInitialize();
	void DashAttackUpdata();
	//投擲
	void ThrowingAttackInitialize();
	void ThrowingAttackUpdata();
	//たたきつけ
	void GroundAttackInitialize();
	void GroundAttackUpdata();
	void InitializeImpact();
	void UpdataImpact();
	//スタン中
	void StanInitialize();
	void StanUpdata();
	//必殺技1
	void SpecialInitialize();
	void SpecialUpdata();
	//必殺技2
	void Special2Initialize();
	void Special2Updata();

	// パーツ親子関係
	void Relationship();

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformRock_;
	WorldTransform worldTransformImpact_;
	WorldTransform worldTransformArea_;
	WorldTransform worldTransformCircleArea_;
	WorldTransform worldTransformColliderImpact_[15];//衝撃波の座標
	WorldTransform worldTransformSter_[3];
	std::unique_ptr<ColliderManager> colliderManager_[15] = {};//衝撃波用の当たり判定
	std::unique_ptr<ColliderManager> colliderRockManager_ = {};//投擲用

	std::vector<ParticleSystem*> particle_;
	std::vector<EmitterSphere> emitter_{};
	std::vector<AccelerationField> accelerationFiled_{};
	Vector3 accelerationVelo_[5];
	Vector3 filedPos_{};

	//アニメーション
	Animations* animation_;
	Animations* animation2_;
	int animationNumber_;

	std::unique_ptr<Model> impactModel_;//衝撃波
	std::unique_ptr<Model> areaModel_;//ダッシュ攻撃エリア
	std::unique_ptr<Model> circleAreaModel_;//投擲攻撃エリア
	std::unique_ptr<Model> sterModel_[3];//混乱時の星

	std::list<EnemyHenchman*> henchmans_;//子分
	Vector3 hmansRenjuPos_;

	//攻撃範囲の座標とフラグ
	Vector3 areaPos_;
	bool areaDraw_;

	//posとrotation
	Vector3 playerPos_ = {};
	Vector3 healerPos_ = {};
	Vector3 renjuPos_ = {};
	Vector3 tankPos_ = {};
	Vector3 tankRotation_ = {};
	bool isDeadPlayer_ = false;
	bool isDeadHealer_ = false;
	bool isDeadRenju_ = false;
	bool isDeadTank_ = false;

	
	//攻撃フラグ
	bool isAttack_ = false;
	bool behaviorAttack_ = false;

	//狙っている相手は誰か
	bool aimPlayer_;
	bool aimHealer_;
	bool aimRenju_;
	bool aimTank_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;
	float sterAngle_[3];

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	BehaviorAttack attack_ = BehaviorAttack::kNomal;
	// 次の振る舞いリクエスト
	std::optional<BehaviorAttack> attackRequest_ = std::nullopt;

	//体力
	float hp_;

	// 速度
	Vector3 velocity_ = {};
	//差分
	Vector3 sub;
	int time_;
	int num_;

	enum TragetNum {
		player,
		healer,
		renju,
		tank,
	};

	//クリアフラグ
	bool clear_;
	//ディゾルブ
	float threshold_ = 0.0f;

	//シェイク
	float shakeTimer_;
	float randX = 0;
	float randZ = 0;

	//動き始めるまでの時間
	int moveTime_ = 0;

	//ゲームが始まったかどうかのフラグ
	bool battleStart_ = false;

	//必殺技フラグ
	bool special_ = false;
	//発作つ技を打てる回数
	int specialCount_ = 1;
};