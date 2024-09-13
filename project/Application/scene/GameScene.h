#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include <Sprite.h>
#include "3d/ModelLoader.h"
#include <camera/followCamera.h>
#include <PostEffects/PostEffect.h>
#include <Skybox.h>
#include <Skydome.h>
#include "Player/DebugPlayer.h"
#include <Ground.h>
#include <Sphere.h>
#include "3d/LineBox.h"
#include "CollisionManager/CollisionManager.h"
#include <ParticleSystem.h>
#include <Enemy/EnemyManager.h>
#include "CollisionManager/CollisionConfig.h"
#include <Enemy/DebugEnemy.h>
#include <Stage.h>
#include <Audio/Audio.h>

class GameScene : public IScene {
public:
	~GameScene();
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void RenderDirect() override;
	void Fade();
	Vector3 GetLocalPosition();
	void CheckAllCollision();

private:
	// 光の数値
	DirectionLight directionLight_{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -2.3f, -1.1f},
		1.0f,
		true,
	};

	ViewProjection viewProjection_;

	// bgmのハンドル
	AudioHelper bgm_;

	std::unique_ptr<Stage> stage_;
	std::unique_ptr<DebugPlayer> debugPlayer_;
	std::unique_ptr<DebugEnemy> debugEnemy_;
	std::unique_ptr<Sprite> backGroundSprite_[6];
	std::unique_ptr<Sprite> spriteBack_;
	Vector2 backGroundPos_ = { 0.0f,0.0f };
	Vector2 shake_;
	bool shakeFlag_;

	//パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_;
	int particleCount_;
	bool particleFlag_;

	bool isFadeOut_;
	bool isFadeIn_;
	bool isFede_;
	float alpha_;

	bool clear_;
	bool over_;

	//PosteEffect
	Grayscale grayscale_;
	Vignetting vignetting_;
	Gaussian smoothing_;
	OutLineStyle outLine_;
	RadialBlur radialBlur_;
	DissolveStyle dissolve_;
	Random random_;
	int32_t isBlur_;
	Vector2 gasianBlur_ = { 0.001f,0.001f };
	BloomStyle bloom_;
	HSVMaterial hsv_;
	bool postEffects[9];

	float env_;
};