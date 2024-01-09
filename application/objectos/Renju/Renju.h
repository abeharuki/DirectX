#pragma once
#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "RenjuBullet.h"

/// <summary>
/// ゲームシーン
/// </summary>
class Renju {

public: // メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Renju();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void Draw(const ViewProjection& view);

	// 移動
	void MoveInitialize();
	void MoveUpdata();

	// ジャンプ
	void JumpInitialize();
	void JumpUpdata();

	// 攻撃
	void AttackInitialize();
	void AttackUpdata();


	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);

	//敵を探す
	void searchTarget(Vector3 enemyPos);

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);


	Vector3 GetWorldPosition();
	Vector3 GetLocalPosition();
	WorldTransform& GetWorldTransform() { return worldTransformBase_; }

		// 弾リストの取得
	const std::list<RenjuBullet*>& GetBullets() const { return bullets_; }


	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

private: // メンバ変数
	WorldTransform worldTransformBase_;
	ViewProjection viewProjection_;
	
	std::unique_ptr<Model> bulletModel_;


	// 目標の角度
	float destinationAngleY_ = 0.0f;

	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kJump, // ジャンプ
		kAttack,//攻撃
		kDead, // 死亡
	};

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	// 速度
	Vector3 velocity_ = {};
	// 味方の押し出し処理
	Vector3 allyVelocity;

	// プレイヤー座標
	float minDistance_ = 10.0f;
	bool followPlayer_ = false;

	//敵を探すフラグ
	bool searchTarget_ = false;

	//弾
	std::list<RenjuBullet*> bullets_;
	// 攻撃時間
	int fireTimer_ = 20;

	Vector3 enemyPos_;
};
