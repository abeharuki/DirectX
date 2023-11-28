#pragma once
#include "BaseCharacter.h"
#include "DirectXCommon.h"
#include "KeyInput.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <GlobalVariables.h>
#include <array>
#include <optional>

// 前方宣言
class LockOn;

class Player : public BaseCharacter {
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
	Player();
	~Player();

	void Initialize(const std::vector<Model*>& models) override;

	void Update() override;

	void Draw(const ViewProjection& viewprojection,bool light) override;

	void MoveInitialize();

	void joyMove();
	void Move();

	void JumpInitialize();
	void JumpUpdata();

	void AttackInitialize();
	void AttackUpdata();

	void DashInitialize();
	void DashUpdata();

	// 調整項目の適用
	void ApplyGlobalVariables();

	// ワールド座標
	Vector3 GetWorldPosition();
	// ローカル座標
	Vector3 GetLocalPosition();

	Vector3 GetHammerWorldPos();

	void Rotate(const Vector3& v);

	// セットPos
	void SetPosition(Vector3 Pos);
	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }
	const WorldTransform& GetHammerWorldTransform() { return worldTransformWW_; }
	const WorldTransform& GetWorldTransform() { return worldTransformBase_; }

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

	// 床のWorldTransformを受け取る
	void SetWorldTransform(const WorldTransform& worldTransform) {
		worldTransformFloor_ = worldTransform;
	}

	void Setparent(const WorldTransform* parent);
	void DeleteParent();

	// 階層構造
	void Relationship(const WorldTransform& worldTransformFloor);
	// パーツ親子関係
	void Relationship();

	// 落下
	void IsFall();

	// 攻撃フラグ
	bool IsAttack() { return workAttack_.isAttack; }
	//最後の攻撃
	bool GetFinalAttack() { return workAttack_.isFinalAttack; }

	// 当たり判定
	void OnCollision();
	void OutCollision();
	// 移動床との当たり判定
	void OnCollisionFloor();
	void OutCollisionFloor();

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHammer_;
	WorldTransform worldTransformFloor_;
	WorldTransform worldTransformWW_;
	const ViewProjection* viewProjection_;



	// 速度
	Vector3 velocity_ = {};
	Vector3 fallSpeed_ = {};
	bool isHit_;
	bool isHitFloor_;
	float dashTimer = 60;
	// 目標の角度
	float destinationAngleY_ = 0.0f;

	// ロックオン
	const LockOn* lockOn_ = nullptr;
	// 振る舞い
	enum class Behavior {
		kRoot,   // 通常状態
		kAttack, // 攻撃中
		kDash,   // ダッシュ
		kJump,   // ジャンプ
		kDead,   // 死亡
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

	// 攻撃の時間
	const uint32_t behaviorAttackTime = 15;
};