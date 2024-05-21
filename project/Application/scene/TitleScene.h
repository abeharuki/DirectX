#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include <memory>
#include "Audio/Audio.h"
#include <Sprite.h>
#include <Sphere.h>
#include <Skydome.h>
#include <Model.h>
#include <numbers>
#include "Animation/Animation.h"
#include "Line.h"
#include <PostEffects/PostEffect.h>
#include <Skybox.h>

class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	//const char* items[3] = { "DirectionLight", "PointLight", "Item 3"};
	//static const int currentItem = 0; // 初期選択アイテムのインデックス


private : // 基本変数
	// 光の数値
	DirectionLight directionLight_{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -2.0f, 0.0f},
		1.0f,
		true,
	};

	PointLight pointLight_ = {
		{1.0f, 1.0f, 1.0f, 1.0f},
		{ 0.0f,3.0f,0.0 },
		1.0f,
		5.0f,
		1.0f,
		true,
	};

	SpotLight spotLight_ = {
		{1.0f, 1.0f, 1.0f, 1.0f},
		{ -2.0f,1.25f,0.0 },
		7.0f,
		{1.0f, -1.0f, 0.0f},
		4.0f,
		2.0f,
		std::cos(std::numbers::pi_v<float>/3.0f),
		true
	};

private:

	Audio* audio_ = nullptr;
	uint32_t audioData_;
	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Line> line_;
	ViewProjection viewProjection_;
	WorldTransform worldTransformSphere_;
	WorldTransform worldTransformBox_;
	WorldTransform worldTransformGround_;
	// 球
	std::unique_ptr<Sphere> sphere_;
	// バニー
	std::unique_ptr<Model> modelBunny_;
	std::unique_ptr<Model> modelGround_;

	std::unique_ptr<Animations>animation_;
	std::unique_ptr<Animations>human_;
	
	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Skybox> skybox_;
	//スプライト
	Vector2 spritePos_;
	Vector2 spriteSize_ = { 250.0f,250.0f };
	bool isSprite_;
	Transform uv;


	Vector3 startPos_;
	Vector3 endPos_;

	Grayscale grayscale_;
	Vignetting vignetting_;
	Smoothing smoothing_;
	bool postEffects[3];
};