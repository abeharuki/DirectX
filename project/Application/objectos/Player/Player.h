#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <array>
#include <optional>
#include "CollisionManager/Collider.h"
#include <Animation/Animation.h>

class Player : public Collider {
public:

	// 攻撃用定数
	struct ConstAttack {
		// 振りかぶり時間
		uint32_t anticipationTime;
		// ため時間
		uint32_t chargeTime;
		// 攻撃振りの時間
		uint32_t swingTime;
		// 硬直時間
		uint32_t recoveryTime;
		// 振りかぶりの移動の速さ
		float anticipationSpeed;
		// ため移動速さ
		float chargeSpeed;
		// 攻撃振りの移動速さ
		float swingSpeed;
	};

	// コンボ
	static const int ComboNum = 3;
	static const std::array<ConstAttack, ComboNum> kConstAttacks_;

public: // メンバ関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void Draw(const ViewProjection& camera);

	// 移動
	void MoveInitialize();
	void MoveUpdata();

	// ジャンプ
	void JumpInitialize();
	void JumpUpdata();

	//ダッシュ
	void DashInitialize();
	void DashUpdata();

	// ノックバック
	void knockInitialize();
	void knockUpdata();

	//攻撃
	void AttackInitialize();
	void AttackUpdata();

	void DeadInitilize();
	void DeadUpdata();

	void Revival();

	// パーツ親子関係
	void Relationship();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;
	void OnCollision(const WorldTransform& worldTransform);
	const Vector3 GetWorldPosition() const override;
	Vector3 GetLocalPosition();
	WorldTransform& GetWorldTransformHammer() { return worldTransformHammer_; }
	WorldTransform& GetWorldTransformHead() { return worldTransformHead_; }
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	WorldTransform& GetWorldTransformCollision() { return worldTransformCollision_; }

	bool IsAttack() { return workAttack_.isAttack; }
	bool IsCombo() { return combo_; }
	bool IsDash() { return dash_; }
	bool IsOver() { return isOver_; }
	bool IsHit() { return hit_; }
	int HitCount() { return hitCount_; }
	void IsDead(bool dead) {
		if (dead) {
			behavior_ = Behavior::kDead;
		};
	}

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

	void SetEnemyAttack(bool attack) { isEnemyAttack_ = attack; }

	//死亡フラグ
	void SetTankDead(bool dead) { tankDead_ = dead; }
	void SetRenjuDead(bool dead) { renjuDead_ = dead; }
	void SetHealerDead(bool dead) { healerDead_ = dead; }

	int GetRevival() { return revivalCount_; }

	bool GetIsDead() {
		if(tankDead_ || renjuDead_ || healerDead_){
			if (preNoAttack_) {
				return true;
			}
			
		}
		
		return false; 
	}

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHammer_;
	WorldTransform worldTransformCollision_;
	std::unique_ptr<Animations>animation_;
	const ViewProjection* viewProjection_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;

	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kJump, // ジャンプ
		kDash, // ダッシュ
		kAttack, //攻撃
		knock,//ノックバック
		kDead, // 死亡
	};

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	// ダッシュ用ワーク
	struct WorkDash {
		// ダッシュ用媒介変数
		uint32_t dashParameter_ = 0;
		// 移動速度
		float dashSpeed = 1.0f;
		// ダッシュの時間
		const uint32_t behaviorDashTime = 15;
	};
	WorkDash workDash_;

	// 攻撃用ワーク
	struct WorkAttack {
		uint32_t attackParameter_ = 0;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
		bool isAttack = false;
		bool isFinalAttack = false;
	};

	WorkAttack workAttack_;

	bool dash_;
	bool combo_;
	bool noAttack_;
	bool preNoAttack_;

	// 攻撃の時間
	const uint32_t behaviorAttackTime = 15;

	//ノックバックの時間とフラグ
	uint32_t nockTime_;
	bool nockBack_;


	// 速度
	Vector3 velocity_ = {};

	bool isOver_;
	float a;

	//体力
	bool hit_;
	int hitCount_;

	bool preHit_;
	bool isHit_;

	//敵の攻撃フラグ
	bool isEnemyAttack_;

	//味方の死亡フラグ
	bool tankDead_;
	bool healerDead_;
	bool renjuDead_;
	int revivalCount_;
};