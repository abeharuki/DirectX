#pragma once
#include <Transition/Transition.h>
#include <Framework/SceneManager.h>

//トランジションエフェクトクラスの定数
namespace TransitionEffectConstants {
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
 * @brief フェイド演出を管理するクラス
 */
class TransitionEffect {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//セッター
	void SetGameOver(bool flag) { over_ = flag; };
	void SetGameClear(bool flag) { clear_ = flag; };

private:
	//ゲームクリアの演出
	void GameClearEffect();

	//ゲームオーバーの演出
	void GameOverEffect();

private:
	// フェードイン・フェードアウト用
	std::unique_ptr<Transition> transition_;

	//ゲームオーバーとクリア画面のスプライト
	std::unique_ptr<Sprite> spriteClear_;
	std::unique_ptr<Sprite> spriteOver_;
	std::unique_ptr<Sprite> spritePushA_;
	Vector3 clearSpritePos_ = { TransitionEffectConstants::kClearSpriteTargetX,-TransitionEffectConstants::kClearSpriteFinalY,0.0f };
	//スプライトのディゾルブ
	float spriteDissolve_ = 1.0f;
	
	bool over_;
	bool clear_;
};