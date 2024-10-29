#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Utilities/RandomGenerator.h"
#include "CollisionManager/Collider.h"
#include <Animation/Animation.h>
#include <CollisionManager/ColliderManager.h>

enum AnimationNumber {
	dashAttack,//ダッシュ攻撃
	death,//死亡
	groundAttack,//地面を殴る攻撃
	nomalAttack,//通常j攻撃
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

	/*-----ゲッター-----*/
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	BehaviorAttack GetBehaviorAttack() { return attack_; }
	Behavior GetBehavior() { return behavior_; }
	Collider* GetCollider(int i) { return colliderManager_[i].get(); }
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

	bool IsBehaberAttack() {
		return  behaviorAttack_;
	}

	/*-----セッター-----*/
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
	void StanInitalize();
	void StanUpdata();


	// パーツ親子関係
	void Relationship();

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformRock_;
	WorldTransform worldTransformImpact_;
	WorldTransform worldTransformArea_;
	WorldTransform worldTransformCircleArea_;
	WorldTransform worldTransformColliderImpact_[15];
	WorldTransform worldTransformSter_[3];
	std::unique_ptr<ColliderManager> colliderManager_[15] = {};
	std::unique_ptr<Animations>animation_;
	int animationNumber_;

	std::unique_ptr<Model> impactModel_;
	std::unique_ptr<Model> areaModel_;
	std::unique_ptr<Model> circleAreaModel_;
	std::unique_ptr<Model> sterModel_[3];
	Vector3 areaPos_;
	bool areaDraw_;

	Vector3 playerPos_ = {};
	Vector3 healerPos_ = {};
	Vector3 renjuPos_ = {};
	Vector3 tankPos_ = {};
	bool isDeadPlayer_ = false;
	bool isDeadHealer_ = false;
	bool isDeadRenju_ = false;
	bool isDeadTank_ = false;

	

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

	bool clear_;
	float threshold_ = 0.0f;

	//シェイク
	float shakeTimer_;
	float randX = 0;
	float randZ = 0;

	//動き始めるまでの時間
	int moveTime_ = 0;

	//ゲームが始まったかどうかのフラグ
	bool battleStart_ = false;
};