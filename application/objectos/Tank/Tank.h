#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// ゲームシーン
/// </summary>
class Tank {

public: // メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Tank();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	// 移動
	void MoveInitialize();
	void MoveUpdata();

	// ジャンプ
	void JumpInitialize();
	void JumpUpdata();

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);

	Vector3 GetWorldPosition();
	Vector3 GetLocalPosition();
	WorldTransform& GetWorldTransform() { return worldTransformBase_; }

	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

private: // メンバ変数
	WorldTransform worldTransformBase_;
	ViewProjection viewProjection_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;

	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kJump, // ジャンプ
		kDead, // 死亡
	};

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	// 速度
	Vector3 velocity_ = {};

	// プレイヤー座標
	float minDistance_ = 10.0f;
	bool followPlayer_ = false;
};
