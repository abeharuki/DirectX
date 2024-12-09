#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include <Sprite.h>
#include <ParticleSystem.h>


/**
 * @file OverScene
 * @brief ゲームオーバーシーンの管理をするクラス
 */
class OverScene : public IScene {
public:
	//シーンの初期化
	void Initialize() override;
	//フレームごとの更新処理
	void Update() override;
	//シーンの描画
	void Draw() override;
	//ポストエフェクトのかからないオブジェクトの描画
	void RenderDirect() override;

private:
	//フェードイン・フェードアウト
	void Fade();
private:
	std::unique_ptr<Sprite> spriteOver_;
	std::unique_ptr<Sprite> spritePushA_;
	// フェードイン・フェードアウト用スプライト
	std::unique_ptr<Sprite> spriteBack_;
	ViewProjection viewProjection_;
	bool isFadeOut_;
	bool isFadeIn_;
	bool isFede_;
	float alpha_;
};