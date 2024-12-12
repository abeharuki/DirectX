#pragma once
#include "2d/Sprite.h"

//トランジションクラスの定数
namespace TransitionConstants {
	const float kAlphaChangeRate = 0.02f;    // フェード時のアルファ値増減量
	const float kMinAlpha = 0.001f;          // 最小アルファ値（フェードイン完了条件）
	const float kMaxAlpha = 1.0f;            // 最大アルファ値（フェードアウト完了条件）
	const Vector2 kSpriteSize = { 1280.0f, 720.0f }; // スプライトのサイズ
}

/**
 * @file GameScene
 * @brief フェイドを管理するクラス
 */
class Transition {
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

	//フェードアウトする
	void ChangeScene();

	/*------------------ゲッター------------------*/
	bool GetFade() { return isFade_; }
	bool GetFadeOut() { return isFadeIn_; }
	bool GetFadeIn() { return isFadeIn_; }
	/*------------------ゲッター------------------*/
	void SetFadeOut(bool flag) { isFadeIn_ = flag; }
	void SetFadeIn(bool flag) { isFadeIn_ = flag; }

private:
	void UpdateFade(float& alpha);
private:

	// フェードイン・フェードアウト用スプライト
	std::unique_ptr<Sprite> spriteBack_;

	bool isFadeOut_;
	bool isFadeIn_;
	bool isFade_;
	float alpha_;
};