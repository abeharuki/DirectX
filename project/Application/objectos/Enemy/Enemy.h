#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "Utilities/RandomGenerator.h"
#include "CollisionManager/Collider.h"
#include <Animation/Animation.h>
#include <CollisionManager/ColliderManager.h>
#include "Editor/editor.h"
#include <AnimationManager.h>
#include <ParticleSystem.h>
#include "EnemyHenchman.h"

//enemyクラスの定数
namespace EnemyConstants {
	//各WorldTransformの初期化変数
	const Vector3 kBaseTranslate = { 0.0f, 0.0f, 10.0f };//ベースpos
	const Vector3 kBaseRotate = { 0.0f, 3.1415f, 0.0f };//ベースの角度
	const Vector3 kBodyScale = { 1.0f, 1.0f, 1.0f };//体のサイズ
	const Vector3 kRockTranslate = { 0.0f, 0.0f, -15000.0f };//投擲オブジェクトの初期pos
	const Vector3 kAreaTranslate = { 0.0f, 0.1f, 0.0f };//ダッシュ攻撃時の範囲を描画するオブジェクトの初期pos
	const Vector3 kAreaScale = { 4.0f, 22.0f, 1.0f };//ダッシュ攻撃時の範囲を描画するオブジェクトのサイズ
	const Vector3 kAreaRotate = { -1.57f,0.0f,0.0f };//ダッシュ攻撃時の範囲を描画するオブジェクトの角度
	const Vector3 kCircleAreaScale = { 3.0f, 3.0f, 3.0f };//投擲攻撃時の範囲を描画するオブジェクトのサイズ
	const Vector3 kShadowScale = { 6.f,6.f,1.0f };//影のサイズ
	const Vector3 kShadowRotate = { -1.571f,0.0f,0.0f };//影の角度


	// バリア関連
	const Vector3 kBarrierScale = { 7.5f, 7.5f, 7.5f };//バリアモデルのサイズ
	const Vector3 kBarrierMaskUVScale = { -1.f, 4.f, 1.f };//バリアのマスク画像のUVサイズ
	const Vector4 kBarrierColor = { 1.0f, 0.0f, 1.0f, 1.0f };//バリアの色
	const float kBarrierThresholdStart = 1.0f;//バリアが全く見えないときのディゾルブ値
	const float kBarrierThresholdIncrement = 0.01f;//バリアが徐々に消えていく時のスピード

	// 行動タイミング関連
	const int kMoveInitTime = 60;//moveに遷移後の初期化タイム
	const int kMoveUpdateTime = 30;//行動が変わるタイミングのタイム
	
	// NomalAttack に関連する定数
	const int kAttackInitTime = 120; // 初期化後の攻撃待機時間 (60 * 2)
	const int kAttackStartTime = 60; // 攻撃開始時刻
	const int kAttackEndTime = 30; // 攻撃終了時刻
	const int kAttackDuration = 20; // 攻撃フレーム数

	// DashAttack に関連する定数
	const int kDashAttackInitTime = 100; // ダッシュ攻撃初期化時のタイマー
	const float kDashAttackFlameTimer = 40.0f; // ダッシュ攻撃アニメーションフレームタイマー
	const int kDashAttackAreaShowTime = 60; // エリア表示開始時刻
	const int kDashAttackRunUpEndTime = 40; // 走るアニメーション終了時刻
	const int kDashAttackRunUpStartTime = 20; // 走るアニメーション開始時刻
	const float kDashVeloZ = 6.0f; // Z方向ダッシュ速度
	
	// ThrowingAttack に関連する定数
	const float kRockInitHeight = 18.0f; // 投げる岩の初期高さ
	const Vector3 kRockInitScale = { 0.0f,0.0f,0.0f};  // 岩の初期スケール
	const float kShakeTimerInit = 60.0f; // シェイクタイマーの初期値
	const Vector3 kRockMaxScale = { 3.0f,3.0f,3.0f }; // 岩の最大スケール
	const float kRockScaleIncrement = 0.1f; // 岩のスケール増加値
	const float kRockLandingHeight = 0.6f; // 岩が地面に到達する高さ

	// InitializeImpact に関連する定数
	const int kImpactColliderCount = 15; // 衝撃波の数
	const float kInitImpactRotationY = 0.2f; // 最初の衝撃波の回転角度
	const float kImpactRotationIncrement = 0.42f; // 衝撃波の回転角度の増加量
	const float kImpactScaleDivisor = 100.0f; // 衝撃波のスケールに使う割り算の値

	// UpdataImpact に関連する定数
	const float kImpactTranslationX[] = { 0.12f, 0.35f, 0.53f, 0.625f, 0.6f, 0.46f, 0.25f, -0.02f, -0.26f, -0.47f, -0.6f, -0.63f, -0.55f, -0.38f, -0.15f };
	const float kImpactTranslationY = 0.0f; // 衝撃波のY方向の移動量（全て同じ）
	const float kImpactTranslationZ[] = { 0.6f, 0.5f, 0.31f, 0.06f, -0.2f, -0.41f, -0.56f, -0.625f, -0.58f, -0.42f, -0.2f, 0.07f, 0.31f, 0.51f, 0.61f };

	// GroundAttack に関連する定数
	const float kGroundAttackAnimationTime = 1.6f; // 地面攻撃のアニメーション時間
	const float kGroundImpactScaleIncrement = 2.0f; // 衝撃波のスケールの増加量
	const float kGroundImpactMaxScale = 100.0f; // 衝撃波の最大スケール
	
	// SpecialBreath に関連する定数
	const int kSpecialBreathMoveTime = 420; // 移動時間
	const float kSpecialBreathAttackStartTime = 180.0f; // 攻撃開始時間
	// Acceleration の設定
	const float kAccelerationValue = 10.0f; // 加速度の値
	const float kFiledPosZ = 5.0f; // FiledPos のZ座標

	// SpecialHenchman に関連する定数
	const int kSpecialHenchmanMoveTime = 600; // 子分のアクション時間
	const float kBarrierInitThreshold = 1.0f; // バリアの初期スレッショルド
	const Vector3 kBarrierScaleIncrease = { 2.f,2.f,2.f }; // バリアのスケール増加値
	const float kBarrierMaxScale = 100.0f; // バリアの最大スケール
	const float kBarrierYTranslate = 6.0f; // バリアのY軸位置
	const float kBarrierRotateSpeed = 0.01f; // バリアの回転速度
	const int kEnemySpawnInterval = 10; // 敵生成の間隔（フレーム単位）
	const float kEnemyHenchmanSpawnRange = 15.0f; // ヘンチマン生成の範囲
	const float kEnemyHenchmanSpawnYOffset = -1.0f; // ヘンチマン生成時のY座標

	// Dead状態に関する定数
	const float kAnimationEndTime = 5.0f;   // アニメーションの終了時間（5秒）
	const float kBarrierThresholdIncreaseRate = 0.01f; // バリアのスレッショルド増加速度
	const Vector3 kEdgeColor = { 0.0f,-1.0f,-1.0f }; // エッジカラー調整値
	const float kThresholdIncreaseRate = 0.001f; // スレッショルドの増加速度

	//必殺技カウントを増やすときに使う定数
	const float kMaxHp = 800.0f;  // 最大HP
	const int kHpThreshold = 50;  // HPの閾値
	
	/*---------------EnemyManagerでのみ使う定数-----------------*/
	//影関連
	const float kShadowHeight = 0.1f;  // 影の高さ
	const float kMaxShadowHeight = 3.9f;  // 最大影の高さ

	//名前のUI関連
	const float kDistanceScaleBase = 17.0f;  // 名前のスケールの基準距離
	const float kDistanceScaleOffset = 14.0f;  // 名前のスケールオフセット

	//HPの色を変わるときの基準定数
	const float kLowHpThreshold = 200.0f;  // 低HP閾値
	const float kMidHpThreshold = 400.0f;  // 中HP閾値

	// ダメージ数関連
	const float kDamagePlayer = 20.0f;  // プレイヤーダメージ
	const float kDamageHealer = 10.0f;  // ヒーラーダメージ
	const float kDamageTank = 10.0f;  // タンクダメージ
	const float kDamageRenjuNormal = 20.0f;  // レンジャーダメージ（通常）
	const float kDamageRenjuSkill = 30.0f;  // レンジャーダメージ（スキル）

	// アルファ値の設定
	const float kAlphaDecreaseRate = 0.08f;  // アルファ値の減少率
	const float kInitAlpha = 2.0f;  // 初期アルファ値
	const float kZeroAlpha = 0.0f;  // 最小アルファ値

	// ランダムな位置調整
	const float kRandomOffsetRange = 3.0f;  // ランダムオフセット範囲
	const float kRandomHeightMin = 3.0f;  // ランダムな高さ最小値
	const float kRandomHeightMax = 6.0f;  // ランダムな高さ最大値

	// しきい値設定
	const float kBarrierThresholdLimit = 0.8f;  // バリアのディゾルブしきい値

	// ビルボード関連
	const float kYPositionOffset = 2.0f;  // Y軸位置オフセット
	const float kBillboardHeight = 9.5f;  // ビルボードの高さ

}

//アニメーションのナンバー
enum AnimationNumber {
	kBreathAttack,//ブレス攻撃
	kDashAttack,//ダッシュ攻撃
	kDeath,//死亡
	kGroundAttack,//地面を殴る攻撃
	kNomalAttack,//通常攻撃
	kRun,//走る
	kRunUp,//ダッシュ攻撃の走り出し
	kStandby,//通常状態
	kSwing,//投擲攻撃
	kThreat,//威嚇
};

// 振る舞い
enum class Behavior {
	kRoot, // 通常状態
	kAttack, // 攻撃
	kStan,//スタン
	kDead, // 死亡
};

enum class BehaviorAttack {
	kNomal,//通常攻撃
	kDash, // ダッシュ攻撃
	kThrowing,//投擲攻撃
	kGround,//地面を殴る攻撃
	kBreath,//必殺技(体力が半分以下になった時に発動)
	kHenchman,//必殺技(体力が半分以下になった時に発動)
};

/**
 * @file Enemy
 * @brief 敵キャラクターを制御するクラス
 */
class Enemy : public Collider {

public: // メンバ関数
	~Enemy();

	//敵の初期化
	void Initialize();

	//毎フレームの更新
	void Update();

	//敵にかかわるobjの描画
	void Draw(const ViewProjection& camera);
	//深度値がないものの描画
	void NoDepthDraw(const ViewProjection& camera);
	//バリアの描画
	void BarrierDraw(const  ViewProjection& camera);

	//当たりは判定
	void OnCollision(Collider* collider) override;


	//必殺技を打てる回数を増やす
	void AddSpecialCount();

	//パーティクルを止める
	void StopParticle() {
		for (int i = 0; i < 5; ++i) {
			particle_[i]->StopParticle();
		}
	}

	/*-----ゲッター-----*/
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	BehaviorAttack GetBehaviorAttack() { return attack_; }
	Behavior GetBehavior() { return behavior_; }
	Behavior GetBehaviorRequest() {
		if (behaviorRequest_) {
			return behaviorRequest_.value();
		}
		return Behavior::kRoot;
	}
	//ディゾルブ
	float GetThreshold() { return barrierThreshold_; }

	bool IsBehaviorRequest(){
		if (behaviorRequest_) {
			return true;
		}
		return false;
	}
	Collider* GetCollider(int i) { return colliderManager_[i].get(); }
	Collider* GetRockCollider() { return colliderRockManager_.get(); }
	// 敵リストの取得
	const std::list<EnemyHenchman*>& GetEnemys() const { return henchmans_; }
	WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	WorldTransform& GetWorldTransformRock() { return worldTransformRock_; }
	WorldTransform& GetWorldTransformArea() { return worldTransformArea_; }

	bool GetAimPlayer() { return aimPlayer_; }
	bool GetAimHealer() { return aimHealer_; }
	bool GetAimRenju() { return aimRenju_; }
	bool GetAimTank() { return aimTank_; }
	bool GetBattle() { return battleStart_; }

	float GetImpactSize() { return worldTransformImpact_.scale.z; }

	bool isAttack() { return isAttack_; };
	bool isClear() { return clear_; };
	bool IsAreaDraw() { return areaDraw_; }
	bool IsBehaberAttack() {return  behaviorAttack_;}
	bool IsSpecial() { return special_;}

	/*-----セッター-----*/
	void SetHP(float hp) { hp_ = hp; }
	void SetLight(DirectionLight directionLight) { 
		animation_->DirectionalLightDraw(directionLight);
		impactModel_->DirectionalLightDraw(directionLight);
		for (int i = 0; i < 3; ++i) {
			//sterModel_[i]->DirectionalLightDraw(directionLight);
		}
	}

	//アニメーションナンバー、フレームタイム、ループ
	void SetAnimationNumber(int num, float flame,bool flag) { animationNumber_ = num; animation_->SetFlameTimer(flame); animation_->SetLoop(flag); }
	//各キャラクターの座標
	void SetPlayerPos(Vector3 pos) { playerPos_ = pos; };
	void SetHealerPos(Vector3 pos) { healerPos_ = pos; };
	void SetRenjuPos(Vector3 pos) { renjuPos_ = pos; };
	void SetTankPos(Vector3 pos) { tankPos_ = pos; };
	//タンクの回転
	void SetTankRotation(Vector3 rotation) { tankRotation_ = rotation; };
	void SethmansRenjuPos(Vector3 pos) { hmansRenjuPos_ = pos; }
	//各キャラの死亡フラグ
	void SetIsDeadPlayer(bool flag) { isDeadPlayer_ = flag; }
	void SetIsDeadHealer(bool flag) { isDeadHealer_ = flag; }
	void SetIsDeadRenju(bool flag) { isDeadRenju_ = flag; }
	void SetIsDeadTank(bool flag) { isDeadTank_ = flag; }
	//バトル開始
	void SetBattleStart(bool flag) { battleStart_ = flag;}
	//レンジャーのスキル
	void SetRenjuSpecial(bool flag) { renjuSpecial_ = flag; }
	void SetDead(bool dead) {
		if (dead) {
			behaviorRequest_ = Behavior::kDead;
		};
	}

	
private:
	// 移動
	void MoveInitialize();
	void MoveUpdata();
	//攻撃
	void AttackInitialize();
	void AttackUpdata();
	//死亡
	void DeadInitilize();
	void DeadUpdata();

	/*-----------------------攻撃レパートリー----------------------*/
	//通常攻撃
	void NomalAttackInitialize();
	void NomalAttackUpdata();
	//ダッシュ
	void DashAttackInitialize();
	void DashAttackUpdata();
	//投擲
	void ThrowingAttackInitialize();
	void ThrowingAttackUpdata();
	//たたきつけ
	void GroundAttackInitialize();
	void GroundAttackUpdata();
	void InitializeImpact();
	void UpdataImpact();
	//必殺技1
	void SpecialBreathInit();
	void SpecialBreathUpdata();
	//必殺技2
	void SpecialHenchmanInit();
	void SpecialHenchmanUpdata();

	// パーツ親子関係
	void Relationship();

	//ターゲットを抽選
	void SelectTarget();
private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformRock_;
	WorldTransform worldTransformImpact_;
	WorldTransform worldTransformArea_;
	WorldTransform worldTransformCircleArea_;
	WorldTransform worldTransformColliderImpact_[15];//衝撃波の座標
	WorldTransform worldTransformBarrier_;

	std::unique_ptr<ColliderManager> colliderManager_[15] = {};//衝撃波用の当たり判定
	std::unique_ptr<ColliderManager> colliderRockManager_ = {};//投擲用

	std::vector<ParticleSystem*> particle_;
	std::vector<EmitterSphere> emitter_{};
	std::vector<AccelerationField> accelerationFiled_{};
	Vector3 accelerationVelo_[5];
	Vector3 filedPos_{};

	//アニメーション
	Animations* animation_;
	Animations* henchman_;
	int animationNumber_;

	std::unique_ptr<Model> impactModel_;//衝撃波
	std::unique_ptr<Model> areaModel_;//ダッシュ攻撃エリア
	std::unique_ptr<Model> circleAreaModel_;//投擲攻撃エリア
	std::unique_ptr<Model> barrierModel_;//バリアモデル
	
	std::list<EnemyHenchman*> henchmans_;//子分
	Vector3 hmansRenjuPos_;

	//攻撃範囲の座標とフラグ
	Vector3 areaPos_;
	bool areaDraw_;
	//エリアのアルファ値
	const float areaAlpha_ = 0.5f;

	//posとrotation
	Vector3 playerPos_ = {};
	Vector3 healerPos_ = {};
	Vector3 renjuPos_ = {};
	Vector3 tankPos_ = {};
	Vector3 tankRotation_ = {};
	bool isDeadPlayer_ = false;
	bool isDeadHealer_ = false;
	bool isDeadRenju_ = false;
	bool isDeadTank_ = false;

	
	//攻撃フラグ
	bool isAttack_ = false;
	bool behaviorAttack_ = false;

	//狙っている相手は誰か
	bool aimPlayer_;
	bool aimHealer_;
	bool aimRenju_;
	bool aimTank_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;
	float sterAngle_[3];

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	BehaviorAttack attack_ = BehaviorAttack::kNomal;
	// 次の振る舞いリクエスト
	std::optional<BehaviorAttack> attackRequest_ = std::nullopt;

	//体力
	float hp_;

	// 速度
	Vector3 velocity_ = {};
	//差分
	Vector3 sub;
	int time_;
	int num_;

	enum TragetNum {
		kPlayer,
		kHealer,
		kRenju,
		kTank,
	};

	//クリアフラグ
	bool clear_;
	//ディゾルブ
	float threshold_ = 0.0f;

	//シェイク(投擲の時の弾が震える演出で使う)
	float shakeTimer_;
	float randX = 0;
	float randZ = 0;

	//動き始めるまでの時間
	int moveTime_ = 0;

	//ゲームが始まったかどうかのフラグ
	bool battleStart_ = false;

	//必殺技フラグ
	bool special_ = false;
	//必殺技を打てる回数
	int specialCount_ = 0;
	//レンジャーの必殺攻撃を食らったかどうか
	bool renjuSpecial_ = false;

	//barrier展開の確認
	bool barrier_ = false;
	//ディゾルブ
	float barrierThreshold_ = 1.f;
	Transform maskUV_;
};