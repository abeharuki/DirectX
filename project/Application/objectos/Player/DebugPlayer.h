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

class Stage;

struct PlayerStatus {

	const char *const kGroupName_ = "Player";

	std::pair<const char *const, float> groundMoveSpeed_ = { "GroundMoveSpeed", 0.2f };
	std::pair<const char *const, float> jumpStrength_ = { "JumpStrength", 0.5f };
	std::pair<const char *const, float> airJumpStrength_ = { "AirJumpStrength", 0.5f };
	std::pair<const char *const, float> gravity_ = { "Gravity", 0.03f };

	std::pair<const char *const, float> stageHeight_ = { "StageHeight", 331.f };
	std::pair<const char *const, float> stageWidth_ = { "StageWidth", 331.f };
	std::pair<const char *const, float> stageeFloor_ = { "StageFloor", -1.5f };

	std::pair<const char *const, float> drawScale_ = { "DrawScale", 128.f };
	std::pair<const char *const, float> drawOffset_ = { "DrawOffset", 64.f };

	void Save() const;
	void Load();

};

class DebugPlayer :public Collider {
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
	~DebugPlayer();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void Draw(const ViewProjection &camera);

	// 移動
	void MoveInitialize();
	void MoveUpdate();

	// ジャンプ
	void JumpInitialize();
	void JumpUpdate();

	// ジャンプ
	void AirJumpInitialize();
	void AirJumpUpdate();

	// 頭突き
	void HeadButtInitialize();
	void HeadButtUpdate();

	// ダッシュ
	void DashInitialize();
	void DashUpdate();

	// 攻撃
	void AttackInitialize();
	void AttackUpdate();

	/// @brief 行列を計算する
	void CalcMatrix();

	/// @brief スプライトにデータを渡す
	void TransfarSprite();

	void OnCollision(Collider *collider) override;
	const Vector3 GetWorldPosition() const override;
	const WorldTransform &GetWorldTransform() const override { return unuseData_; }
	void SetViewProjection(const ViewProjection *viewProjection) {
		pViewProjection_ = viewProjection;
	}
	void SetStage(Stage *stage) { pStage_ = stage; }
private: // メンバ変数
	Transform transform_;
	Matrix4x4 transMat_;
	WorldTransform unuseData_;
	const ViewProjection *pViewProjection_;
	std::unique_ptr<Animations>animation_;
	std::unique_ptr<Sphere> sphere_;

	// ステージへの参照
	Stage *pStage_ = nullptr;

	// 目標の角度
	float destinationAngleY_ = 0.0f;

	// 振る舞い
	enum class Behavior {
		kRoot,		// 通常状態
		kJump,		// ジャンプ
		kAirJump,	// 空中ジャンプ
		kDash,		// ダッシュ
		kHeadButt,	// 頭突き
		kAttack,	// 攻撃
		knock,		// ノックバック
		kDead,		// 死亡

		kMaxCount	// ビヘイビアの数
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

	// ジャンプの時間
	uint32_t behaviorJumpTime = 20;

	// 速度
	Vector3 velocity_ = {};

	// プレイヤのステータス
	PlayerStatus playerStatus_;

	std::unique_ptr<Sprite> sprite_ = nullptr;

	Transform uvTransform_;

};
