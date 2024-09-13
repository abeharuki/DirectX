#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include <memory>
#include "Audio/Audio.h"
#include <Sprite.h>
#include <Model.h>
#include <Ground.h>
#include <Skydome.h>
#include <camera/followCamera.h>
#include <Player/PlayerManager.h>
#include <Enemy/EnemyManager.h>
#include <Healer/HealerManager.h>
#include <Tank/TankManager.h>
#include <Renju/RenjuManager.h>
#include <Animation/Animation.h>
#include "PostEffects/PostEffect.h"
#include <Line.h>
#include <ModelLoader.h>

class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void RenderDirect() override;
	void Fade();
	~TitleScene() override;

	void cameraMove();

private:

	void StartTransition();

private: // 基本変数
	// 光の数値
	DirectionLight directionLight_{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -2.0f, 0.0f},
		1.0f,
		true,
	};

private:

	// bgmのハンドル
	AudioHelper bgm_;

	AudioHelper pressStart_;

	// フェードイン・フェードアウト用スプライト
	std::unique_ptr<Sprite> spriteBack_;
	std::unique_ptr<Sprite> backGroundSprite_;
	std::unique_ptr<Sprite> titleText_;


	Vector2 pos_;
	bool rule_;

	bool isFadeOut_;
	bool isFadeIn_;
	bool isFede_;
	float alpha_;

	float a_;
};