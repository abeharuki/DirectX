#pragma once
#include "Framework/IScene.h"
#include "Skydome.h"
#include <Player/PlayerManager.h>
#include "camera/followCamera.h"
#include <Enemy/EnemyManager.h>
#include <Healer/Healer.h>
#include <Tank/Tank.h>
#include <Renju/Renju.h>
#include "CollisionManager/CollisionManager.h"
#include <Command/Command.h>
#include <ModelLoader.h>
#include <Transition/TransitionEffect.h>

//ゲームシーンクラスの定数
namespace GameSceneConstants {
	// ラジアルブラーの初期設定
	const float kRadialBlurInitWidth = 0.01f;//ブラーの掛ける強さ
	const float kRadialBlurInitRotation = 0.1f;//ブラーの回転

	// ラジアルブラーの最大幅
	const float kMaxRadialBlurWidth = 0.9f;//ブラーのmax強さ

	// カメラ方向遷移の最大時間
	const int kCameraDirectionTimeMax = 10;

	// 各キャラクターの初期位置
	const float kHealerInitPos = 6.0f;
	const float kRenjuInitPos = -3.0f;
	const float kTankInitPos = 0.0f;

	// 追従カメラの初期位置
	const Vector3 kFollowCameraInit = { 3.0f,0.0f,-35.0f };

	// バトル開始のZ軸閾値
	const float kBattleStartZThreshold = -49.9f;

	// スタート時の敵のアニメーションフレームタイム
	const float kEnemyAnimationFlame = 25.0f;

	//ゲームオバーディゾルブ関連
	const float kDissolveThresholdIncrement = 0.01f;    //ディゾルブの閾値増加量
	const Vector3 kDissolveEdgeColor = { 1.0f,0.0f,0.0f };//エッジの色

	//ゲームクリアのスプライト関連
	const float kInitialClearSpriteY = -1.0f;// 初期クリアスプライトのY座標
	const float kClearSpriteTargetX = 50.0f; // クリアスプライトの目標X座標
	const float kClearSpriteFinalY = 720.0f; // クリアスプライトの最終Y座標
}

/**
 * @file GameScene
 * @brief ゲームプレイを管理するクラス
 */
class GameScene : public IScene {
public: // メンバ関数
	//シーンの初期化
	void Initialize() override;
	//フレームごとの更新処理
	void Update() override;
	//シーンの描画
	void Draw() override;
	//ポストエフェクトのかからないオブジェクトの描画
	void RenderDirect() override;

private:
	
	//バトル開始の演出
	void BattleBegin();

	//エフェクトの管理
	void Effect();

	//回復の適用
	void ApplyHealing();

	//各キャラクターの情報を敵に渡す
	void ConfigureEnemyState();

	//カメラやライティングなどの視覚情報の更新とセット
	void ViewConfig();

	//当たり判定
	void CheckAllCollision();
private://基本変数
	// 光の数値
	DirectionLight directionLight_{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -2.0f, 0.0f},
		1.0f,
		true,
	};

private: // メンバ変数

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;
	ViewProjection viewProjection_;

	// 天球
	std::unique_ptr<Skydome> skydome_;
	// 天球3Dモデル
	std::unique_ptr<Model> modelSkydome_;
	//ローダー
	std::unique_ptr<ModelLoader> loader_;
	
	//遷移エフェクトと遷移を管理
	std::unique_ptr<TransitionEffect> transitionEffect_;

	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	//プレイヤー
	std::unique_ptr<PlayerManager> playerManager_;
	// 敵
	std::unique_ptr<EnemyManager> enemyManager_;

	//タンク
	std::unique_ptr<Tank> tank_;
	//ヒーラー
	std::unique_ptr<Healer> healer_;
	//レンジャー
	std::unique_ptr<Renju> renju_;

	//コマンド
	std::unique_ptr<Command> command_;

	//ラジアルブラー
	RadialBlur radialBlur_;
	//戦闘移行演出
	bool battle_ = false;
	//バトル開始時のカメラ演出
	bool cameraDirection_ = false;
	int cameraDirectionTime_;

	//味方AIキャラクター
	std::vector<AllyAICharacter*> characters;

};