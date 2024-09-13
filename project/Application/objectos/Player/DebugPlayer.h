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
#include <GlobalVariables.h>
#include <Audio/Audio.h>

class Stage;
// プレイヤの前方宣言
class DebugPlayer;

struct JsonLoadHelper
{
	const std::string groupName_;
	GlobalVariables *const pGVal_ = GlobalVariables::GetInstance();

	template<typename T>
	void operator>>(std::pair<const char *const, T> &value) const {
		pGVal_->GetValue<T>(groupName_, value.first, &value.second);
	}

	template<typename T>
	void operator<<(const std::pair<const char *const, T> &value) {
		pGVal_->SetValue(groupName_, value.first, value.second);
	}

};

struct PlayerStatus {

	const char *const groupName_;

	std::pair<const char *const, float> groundMoveSpeed_ = { "GroundMoveSpeed", 0.2f };
	std::pair<const char *const, float> jumpStrength_ = { "JumpStrength", 0.5f };
	std::pair<const char *const, float> airJumpStrength_ = { "AirJumpStrength", 0.5f };
	std::pair<const char *const, float> gravity_ = { "Gravity", 0.03f };

	std::pair<const char *const, float> stageHeight_ = { "StageHeight", 331.f };
	std::pair<const char *const, float> stageWidth_ = { "StageWidth", 331.f };
	std::pair<const char *const, float> stageeFloor_ = { "StageFloor", -1.5f };

	std::pair<const char *const, float> drawScale_ = { "DrawScale", 128.f };
	std::pair<const char *const, float> drawOffset_ = { "DrawOffset", 64.f };

	std::pair<const char *const, int32_t> knockBackFlame_ = { "KnockBackFlame", 30 };
	std::pair<const char *const, Vector3> knockBackPower = { "KnockBackPower", {0.5f, 0.2f,0.f} };
	std::pair<const char *const, int32_t> maxHealth_ = { "MaxHealth", 10 };

	std::pair<const char *const, int32_t> deadFlame = { "DeadFlame", 30 };

	void Save() const;
	void Load();
	/// @brief 読み書きのヘルパを生成
	/// @return jsonへの書き込みを簡略化するヘルパ
	JsonLoadHelper GenerateLoadHelper() const { return JsonLoadHelper(groupName_); }
};


/// @brief プレイヤの体力を描画するマネージャ
class PlayerHealthDrawer {
public:
	PlayerHealthDrawer() = default;
	~PlayerHealthDrawer() = default;

public:

	/// @brief プレイヤステータスを代入する
	/// @param player プレイヤへの参照
	/// @param playerStatus プレイヤのステータス情報群
	void SetPlayer(const DebugPlayer *player, const PlayerStatus *playerStatus) { pPlayer_ = player; pPlayerStatus_ = playerStatus; }

	void Init();

	void Update();

	void Draw();

private:

	/// @brief ハートの座標を計算する
	void CalcPos();

public:
	void Save() const;
	void Load();

	/// @brief 読み書きのヘルパを生成
	/// @return jsonへの書き込みを簡略化するヘルパ
	JsonLoadHelper GenerateLoadHelper() const { return JsonLoadHelper(groupName_); }

private:

	// ImGuiに表示されるグループ名
	static inline const char *const groupName_ = "PlayerHealthDrawer";

	using AnimateSprite = std::pair<Sprite *, float>;

private:
	// Json保存変数 //

	std::pair<const char *const, Vector3> vScale_ = { "Scale", {32.f ,32.f ,0.f} };
	std::pair<const char *const, float> vOffsetY_ = { "OffsetY", -48.f };
	std::pair<const char *const, float> vDistanceX_ = { "DistanceX", 24.f };

private:
	const PlayerStatus *pPlayerStatus_ = nullptr;
	const DebugPlayer *pPlayer_ = nullptr;

	// 描画するスプライトのデータ
	std::unique_ptr<AnimateSprite[]> healthSpriteData_;
	std::span<AnimateSprite> healthSpriteList_;

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

	/// @brief アニメーションを実際に行う
	/// @param layer 縦方向のレイヤー
	/// @param span アニメーションの秒間
	/// @param begin アニメーションの開始地点
	/// @param end アニメーションの終了地点
	/// @param isRoop ループするかどうか
	/// @return 終端に達しているかどうか
	bool AnimUpdate(const uint32_t layer, const uint32_t span, const uint32_t begin, const uint32_t end, bool isRoop);

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

	// ノックバック
	void KnockBackInitialize();
	void KnockBackUpdate();

	// 死亡状態
	void DeadInitialize();
	void DeadUpdate();

	/// @brief 行列を計算する
	void CalcMatrix();

	/// @brief スプライトにデータを渡す
	void TransfarSprite();

	/// @brief ステージの棒との当たり判定の検知
	void StageBarCollision();

	/// @brief 棒にあたった場合の処理
	/// @param index 棒の番号
	void OnCollision(const uint32_t index);

	void OnCollision(Collider *collider) override;
	const Vector3 GetWorldPosition() const override;
	const Vector3 GetTransformTranslate() { return transform_.translate; }
	const WorldTransform &GetWorldTransform() const override { return unuseData_; }
	void SetViewProjection(const ViewProjection *viewProjection) {
		pViewProjection_ = viewProjection;
	}
	void SetStage(Stage *stage) { pStage_ = stage; }
	bool GetHitBlock() const { return hitBlock_; }

	/// @brief ステージ上のプレイヤの位置を返す
	/// @return ステージ上においてのX座標
	float GetOnStagePosX() const;

	/// @brief プレイヤがステージのY座標からどれだけの割合にいるのか
	/// @return Yの高さ { 0.f ~ 1.f }
	float GetOnStagePosYPercent() const;

	/// @brief ゲームオーバーになったかどうか
	/// @return ゲームオーバーになったらtrue
	bool IsGameOver() const;

	int32_t GetHealth() const { return health_; }
	float GetHealthParcent() const { return static_cast<float>(health_) / playerStatus_.maxHealth_.second; }

private: // メンバ変数
	Transform transform_;
	Matrix4x4 transMat_;
	// ステージにおいての座標
	Vector2 stagePos_;
	WorldTransform unuseData_;
	const ViewProjection *pViewProjection_;
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
		kKnockBack,	// ノックバック
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
	bool hitBlock_;

	// ジャンプの時間
	uint32_t behaviorJumpTime = 20;

	// 速度
	Vector3 velocity_ = {};

	// プレイヤのステータス
	PlayerStatus playerStatus_{ "Player" };

	// プレイヤのスプライトデータ
	std::unique_ptr<Sprite> sprite_ = nullptr;

	// 体力
	int32_t health_;

	// プレイヤのテクスチャUV座標
	Transform uvTransform_;
	// アニメーションのフレームカウント
	uint32_t animFlame_;

	// プレイヤの体力の描画
	std::unique_ptr<PlayerHealthDrawer> healthDrawer_;

	AudioHelper headButtSound_;

	AudioHelper jumpSound_;

};
