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
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void RenderDirect() override;

private:
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