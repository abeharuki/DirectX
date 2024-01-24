#pragma once
#include "Framework/IScene.h"
#include "KeyInput.h"
#include <memory>
#include "Audio/Audio.h"
#include <Sprite.h>
#include <Sphere.h>
#include <Model.h>

class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private : // 基本変数
	// 光の数値
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0};
	Vector3 direction_ = {0.0f, -2.0f, 0.0f};
	float intensity_ = 1.0f;


private:

	Audio* audio_ = nullptr;
	uint32_t audioData_;
	std::unique_ptr<Sprite> sprite_;
	ViewProjection viewProjection_;
	WorldTransform worldTransformSphere_;
	// 球
	std::unique_ptr<Sphere> sphere_;
	// バニー
	std::unique_ptr<Model> modelBunny_;

	Vector2 size;
};