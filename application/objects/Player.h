#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "BaseCharacter.h"
#include <GlobalVariables.h>
#include <optional>


/// <summary>
/// ゲームシーン
/// </summary>
class Player : public BaseCharacter {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::vector<Model*>& models) override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewprojection) override;

	void joyMove();

	void Move();

	void Jump();

	void AttackInitialize();
	void AttackUpdata();

	void DashInitialize();
	void DashUpdata();

	// 調整項目の適用
	void ApplyGlobalVariables();

	//ワールド座標
	Vector3 GetWorldPosition();
	//ローカル座標
	Vector3 GetLocalPosition();

	Vector3 GetHammerWorldPos();

	// セットPos
	void SetPosition(Vector3 Pos);
	
	const WorldTransform& GetHammerWorldTransform() { return worldTransformWW_; }
	const WorldTransform& GetWorldTransform() { return worldTransformBase_; }

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

	//床のWorldTransformを受け取る
	void SetWorldTransform(const WorldTransform& worldTransform) {
		worldTransformFloor_ = worldTransform;
	}

	void Setparent(const WorldTransform* parent);
	void DeleteParent();

	//階層構造
	void Relationship(const WorldTransform& worldTransformFloor);
	// パーツ親子関係
	void Relationship();

	//落下
	void IsFall();

	//攻撃フラグ
	bool IsAttack() { return attack; }

	//当たり判定
	void OnCollision();
	void OutCollision();
	//移動床との当たり判定
	void OnCollisionFloor();
	void OutCollisionFloor();

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHammer_;
	WorldTransform worldTransformFloor_;
	WorldTransform worldTransformWW_;
	const ViewProjection* viewProjection_;
	float upSpeed_;
	float fallSpeed_;
	bool jump_;
	bool isHit_;
	bool isHitFloor_;
	float dashTimer = 60;
	// 目標の角度
	float destinationAngleY_ = 0.0f;
	

	// 振る舞い
	enum class Behavior {
		kRoot,   // 通常状態
		kAttack, // 攻撃中
		kDash,   // ダッシュ
		kDead,   // 死亡
	};

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
	// ダッシュ用ワーク
	struct WorkDash {
		// ダッシュ用媒介変数
		uint32_t dashParameter_ = 0;
	};
	WorkDash workDash_;
	// 移動速度
	float kDashSpeed = 1.0f;
	// ダッシュの時間
	const uint32_t behaviorDashTime = 15;

	// 攻撃フラグ
	bool attack = false;
	float attackSpeed = -0.1f;
	float attackTime = 1.0f;
	float changeTime = 1.0f;
};
