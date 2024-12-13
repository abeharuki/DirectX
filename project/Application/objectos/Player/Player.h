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

//プレイヤークラスの定数
namespace PlayerConstants{
	// 位置・スケール・回転に関する設定
	const Vector3 kPlayerInitPosition = { 3.0f,0.0f,0.0f};  // プレイヤーの初期位置（X座標）
	const Vector3 kWeaponRotate = { -0.7f,0.0f,1.7f };//武器の初期角度
	const Vector3 kWeaponCollisionScale = { 0.1f, 1.0f, 0.1f };  // プレイヤーのコリジョンスケール（X, Y, Z）
	const Vector3 kWeaponCollisionTranslate = { 0.0f, 2.0f,0.0f };  // プレイヤーのコリジョンのY座標
	const Vector3 kPlayerNumScale = { 0.5f, 0.5f, 0.5f };  // プレイヤーのダメージ表示スケール（X, Y, Z）

	// プレイヤーのダメージ関連
	const float kDamageAlphaInitValue = 2.0f;  // プレイヤーが食らったときダメージ表示の初期アルファ値（透明度）
	const float kDamageAlphaDecrement = 0.08f;  // ダメージ表示のアルファ値の減少速度

	// プレイヤーの攻撃関連
	const float kAttackEnemyDamageNormal = 10.0f;  // 通常攻撃,投擲、ダッシュのダメージ値
	const float kAttackEnemyDamageBreath = 20.0f;  // ブレス攻撃,地面を殴るのダメージ値
	const float kAttackEnemyDamageHenchman = 40.0f;  // 子分攻撃のダメージ値
	const float kPlayerDamageDisplayHeight = 2.0f;  // ダメージ表示のY座標オフセット
	const float kAttackRange = 5.0f;  // 攻撃範囲（敵との距離）
	// 攻撃関連
	const float kAttackAnimationStartTime = 0.8f;  // 攻撃アニメーションが開始される時間
	const float kAttackAnimationEndTime = 1.6f;    // 攻撃アニメーションが終了する時間

	// プレイヤーのHP,MP関連
	const float kPlayerMaxHP = 100.0f;  // プレイヤーの最大HP
	const float kPlayerMaxMP = 100.0f;  // プレイヤーの最大MP
	
	// プレイヤーの行動タイミング関連
	const float kFlameTimeDefault = 30.0f;  // プレイヤーのフレーム時間（デフォルト）

	// 死亡関連
	const float kDeadThresholdIncrement = 0.01f;    // 死亡時の閾値増加量
	const float kDeadAnimationThreshold = 1.0f;     // 死亡アニメーション終了の閾値
	const float kDeadHP = 0.0f;                     // 死亡時のHP
	const Vector3 kDeadEdgeColor = { 0.0f, -1.0f, -1.0f }; // 死亡時のエッジカラー

	/*---------------PlayerManagerでのみ使う定数-----------------*/
	// プレイヤー関連
	const float kPlayerHpLowThreshold = 20.0f; // HPが低い閾値（赤色に変化）
	const float kPlayerHpMediumThreshold = 50.0f; // HPが中間の閾値（オレンジ色に変化）

	//影関連
	const float kShadowHeight = 0.11f;  // 影の高さ
	const float kMaxShadowHeight = 3.9f;  // 最大影の高さ
	const Vector3 kShadowScale = { 1.8f,1.8f,1.0f };//影のサイズ
	const Vector3 kShadowRotate = { -1.571f,0.0f,0.0f };//影の角度

	// スプライト関連
	const Vector2 kHpSpriteSize = { 100.0f, 10.0f };   // HPバーのサイズ
	const Vector2 kMpSpriteSize = { 100.0f, 10.0f };   // MPバーのサイズ
	const Vector2 k1PSize = { 93.0f, 85.0f };        // 1Pスプライトのサイズ
	const Vector2 kHSize = { 35.0f, 35.0f };            // Hスプライトのサイズ
	const Vector2 kMSize = { 35.0f, 35.0f };            // Mスプライトのサイズ
	const Vector2 kNameSize = { 106.0f, 50.0f };        // 名前スプライトのサイズ
	const Vector2 kHpNumSize = { 36.0f, 36.0f };  // HP数字のサイズ
	const Vector2 kMpNumSize = { 36.0f, 36.0f };  // MP数字のサイズ
	const Vector2 kHpSpritePos = { 1106.0f,405.0f };   // HPバーの位置
	const Vector2 kMpSpritePos = { 1106.0f,430.0f };   // MPバーの位置
	const Vector2 k1PPos = { 995.0f,373.0f };        // 1Pスプライトの位置
	const Vector2 kHPos = { 1097.0f,383.0f };            // Hスプライトの位置
	const Vector2 kMPos = { 1097.0f,408.0f };            // Mスプライトの位置
	const Vector2 kNamePos = { 995.0f,363.0f };        // 名前スプライトの位置

	// HP・MPナンバーの位置関連
	const float kHpNumPosOffset = 16.0f;  // HP数字の位置のオフセット
	const float kMpNumPosOffset = 16.0f;  // MP数字の位置のオフセット
	const Vector2 kHpNumInitPos = { 1172.0f, 385.0f }; // 初期HP番号スプライトの位置
	const Vector2 kMpNumInitPos = { 1172.0f, 410.0f }; // 初期MP番号スプライトの位置
}

/**
 * @file Player
 * @brief プレイヤーが操作するキャラクターを制御するクラス
 */
class Player : public Collider {
public:

	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kJump, // ジャンプ
		kDash, // ダッシュ
		kAttack, //攻撃
		kDead, // 死亡
	};

public: // メンバ関数
	~Player();
	//プレイヤーの初期化
	void Initialize();

	//毎フレームの更新
	void Update();

	//プレイヤーにかかわるobjの描画
	void Draw(const ViewProjection& camera);
	//深度値がないものの描画
	void NoDepthDraw(const ViewProjection& camera);

	
	//位置のの初期化・更新
	void InitPos();
	void UpdatePos() {
		worldTransformBase_.UpdateMatrix();
		worldTransformHead_.TransferMatrix();
		worldTransformHammer_.TransferMatrix();
		worldTransformNum_.TransferMatrix();
	}

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;
	//味方が展開したバリアの中にいるか
	void BarrierRange();

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
	bool IsAttack() { return isAttack; }
	bool IsCombo() { return combo_; }
	bool IsDash() { return dash_; }
	bool IsOver() { return isOver_; }
	bool IsDead() { return isDead_; }
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
	void SetBarrierPos(Vector3 pos) { barrierPos_ = pos; }

	//死亡フラグ
	void SetTankDead(bool dead) { tankDead_ = dead; }
	void SetRenjuDead(bool dead) { renjuDead_ = dead; }
	void SetHealerDead(bool dead) { healerDead_ = dead; }
	//バトル開始
	void SetBattleStart(bool flag) { battleStart_ = flag; }
	
private:
	// 移動の初期化・更新
	void MoveInitialize();
	void MoveUpdata();

	// ジャンプの初期化・更新
	void JumpInitialize();
	void JumpUpdata();

	//ダッシュの初期化・更新
	void DashInitialize();
	void DashUpdata();

	//攻撃の初期化・更新
	void AttackInitialize();
	void AttackUpdata();

	//死亡の初期化・更新
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
		kAnimeAttack,//攻撃
		kJump,//ジャンプ
		kRun,//移動
		kStandby,//待機
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
	bool isAttack = false;

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
	float hp_ = PlayerConstants::kPlayerMaxHP;
	//マジックポイント
	float mp_ = PlayerConstants::kPlayerMaxMP;

	bool preHit_;
	bool isHit_;

	//味方の死亡フラグ
	bool tankDead_;
	bool healerDead_;
	bool renjuDead_;
	int revivalCount_;

	//barrierのポジション
	Vector3 barrierPos_;

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