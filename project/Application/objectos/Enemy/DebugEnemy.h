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
#include "PostEffects/PostEffect.h"
#include <Sphere.h>
#include "Player/DebugPlayer.h"

class DebugEnemy{
public: // メンバ関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugEnemy();


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

	//攻撃
	void AttackInitialize();
	void AttackUpdata();


	void SetViewProjection(const ViewProjection* viewProjection) {viewProjection_ = viewProjection;}
	void SetDebugPlayer(DebugPlayer* debugPlayer) { debugPlayer_ = debugPlayer; }
	bool GetAttack() { return attack_; }
	Vector2 GetPos() { return Vector2{ transformBase_.translate.x,transformBase_.translate.y }; }
	Vector2 GetScale() {return Vector2{ transformBase_.scale.x,transformBase_.scale.y };}

private: // メンバ変数
	Transform transformBase_;
	const ViewProjection* viewProjection_;
	std::unique_ptr<Sprite> enemySprite_;

	DebugPlayer* debugPlayer_;

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

	bool attack_;
	bool jump_;

	// ジャンプの時間
	uint32_t behaviorJumpTime = 30;
	//攻撃までの時間(クールタイム)
	uint32_t behaviorAttackTime = 60;
	//移動可能になるまでの時間
	uint32_t behaviorMoveTime = 20;

	// 速度
	Vector3 velocity_ = {};
	
};