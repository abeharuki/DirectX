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
#include "Command/Command.h"
#include "Enemy/Enemy.h"

class Player : public Collider {
public:

	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kJump, // ジャンプ
		kDash, // ダッシュ
		kAttack, //攻撃
		knock,//ノックバック
		kDead, // 死亡
	};

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
	void NoDepthDraw(const ViewProjection& camera);

	
	//位置の初期化
	void InitPos();
	void UpdatePos() {
		worldTransformBase_.UpdateMatrix();
		worldTransformHead_.TransferMatrix();
		worldTransformHammer_.TransferMatrix();
		worldTransformNum_.TransferMatrix();
	}

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;

	/*-----ゲッター-----*/
	const Vector3 GetWorldPosition() const override;
	Vector3 GetLocalPosition();
	WorldTransform& GetWorldTransformHammer() { return worldTransformHammer_; }
	WorldTransform& GetWorldTransformHead() { return worldTransformHead_; }
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	WorldTransform& GetWorldTransformCollision() { return worldTransformCollision_; }
	bool GameStart() { return gameStart_; }
	//こうげきフラグ
	bool IsRoot() { return root_; }
	bool IsAttack() { return workAttack_.isAttack; }
	bool IsCombo() { return combo_; }
	bool IsDash() { return dash_; }
	bool IsOver() { return isOver_; }
	bool IsOuto() { return auto_; }
	float GetHp() { return hp_; }
	float GetMp() { return mp_; }
	int GetRevival() { return revivalCount_; }

	bool GetIsDead() {
		if (tankDead_ || renjuDead_ || healerDead_) {
			if (preNoAttack_) {
				return true;
			}

		}

		return false;
	}

	//状態
	Behavior GetBehavior() { return behavior_; }


	/*-----セッター-----*/
	bool SetAttack(bool flag, int i) { return attackType_[i] = flag; }
	void SetHeal(float heal) { 
		if (!isDead_) {
			hp_ += heal;
		}
	}
	void SetViewProjection(const ViewProjection* viewProjection) {viewProjection_ = viewProjection;}
	void SetCamera(const ViewProjection& camera) {camera_ = camera;}
	//敵の情報の受け取り
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }
	void SetEnemyLength(Vector3 pos){
		// 敵の座標までの距離
		length_ = Math::Length(Math::Subract(pos, worldTransformBase_.translate));
	}

	void SetLight(DirectionLight directionLight) { animation_->DirectionalLightDraw(directionLight); }

	//死亡フラグ
	void SetTankDead(bool dead) { tankDead_ = dead; }
	void SetRenjuDead(bool dead) { renjuDead_ = dead; }
	void SetHealerDead(bool dead) { healerDead_ = dead; }
	void SetBattleStart(bool flag) { battleStart_ = flag; }
	
private:
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

	//死亡
	void DeadInitilize();
	void DeadUpdata();

	// パーツ親子関係
	void Relationship();
private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHammer_;
	WorldTransform worldTransformCollision_;

	//ダメージ表示
	WorldTransform worldTransformNum_;
	std::unique_ptr<Model> damageModel_;
	float alpha_;
	Vector3 numMove_;

	Animations* animation_;
	int animationNumber_;
	enum AnimationNumber {
		animeAttack,//攻撃
		jump,//ジャンプ
		run,//移動
		standby,//待機
	};

	float flameTime_;

	const ViewProjection* viewProjection_;
	ViewProjection camera_;
	std::unique_ptr<LineBox> linebox_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;

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

	bool root_;
	bool dash_;
	bool combo_;
	bool noAttack_;
	bool preNoAttack_;
	bool auto_;

	// 攻撃の時間
	const uint32_t behaviorAttackTime = 15;

	//ノックバックの時間とフラグ
	uint32_t nockTime_;
	bool nockBack_;


	// 速度
	Vector3 velocity_ = {};

	//死亡時のフラグ
	bool isDead_ = false;
	bool isOver_;
	float threshold_;

	//体力
	float hp_ = 100.0f;
	//マジックポイント
	float mp_ = 100.0f;

	bool preHit_;
	bool isHit_;

	//味方の死亡フラグ
	bool tankDead_;
	bool healerDead_;
	bool renjuDead_;
	int revivalCount_;

	//コマンドこうげきを受け取るよう
	//攻撃フラグ
	std::vector<bool> attackType_;

	float length_ = 0.0f;

	//敵の情報
	Enemy* enemy_;

	//ゲームスタートフラグ
	bool gameStart_;
	//バトルが始まったかどうかのフラグ
	bool battleStart_ = false;
};