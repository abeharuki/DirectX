#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include <memory>
#include "Audio/Audio.h"
#include <Sprite.h>
#include <Sphere.h>
#include <Model.h>
#include <numbers>

/**
 * @file LaoadScene
 * @brief ゲームのロードシーンを管理するクラス
 */
class LoadScene : public IScene {
public:
	//シーンの初期化
	void Initialize() override;
	//フレームごとの更新処理
	void Update() override;
	//シーンの描画
	void Draw() override;
	//ポストエフェクトのかからないオブジェクトの描画
	void RenderDirect() override;

private: // 基本変数
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
		std::cos(std::numbers::pi_v<float> / 3.0f),
		true
	};

private:

	Audio* audio_ = nullptr;
	uint32_t audioData_;
	std::unique_ptr<Sprite> backSprite_;
	std::unique_ptr<Sprite> nowLoadingSprite_;
	std::unique_ptr<Sprite> loadSprite_;
	ViewProjection viewProjection_;
	

	//スプライト
	Vector2 spritePos_;
	Vector2 spriteSize_ = { 250.0f,250.0f };
	bool isSprite_;
	Transform uv;

};