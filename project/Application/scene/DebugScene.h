#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include <Sprite.h>
#include "3d/ModelLoader.h"
#include <camera/followCamera.h>
#include <PostEffects/PostEffect.h>
#include <Skybox.h>
#include <Skydome.h>

class DebugScene: public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void CameraMove();
	Vector3 GetLocalPosition();
private:
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	ViewProjection viewProjection_;
	WorldTransform worldTransform_;
	WorldTransform worldTransformSkybox_;
	WorldTransform worldTransformAnimation_;
	std::unique_ptr<Model> modelBunny_;
	std::unique_ptr<Animations>animation_;

	// 速度
	Vector3 velocity_ = {};
	// 目標の角度
	float destinationAngleY_ = 0.0f;

	//ローダー
	std::unique_ptr<ModelLoader> loader_;

	std::unique_ptr<Skybox> skybox_;

	//PosteEffect
	Grayscale grayscale_;
	Vignetting vignetting_;
	Smoothing smoothing_;
	bool postEffects[3];

	float env_;
};