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

class DebugScene: public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void RenderDirect() override;
	void CameraMove();
	Vector3 GetLocalPosition();
private:
	// 光の数値
	DirectionLight directionLight_{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -2.0f, 0.0f},
		1.0f,
		true,
	};

	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	ViewProjection viewProjection_;
	WorldTransform worldTransform_;
	WorldTransform worldTransformSkybox_;
	WorldTransform worldTransformAnimation_;
	WorldTransform worldTransformSphere_;
	WorldTransform worldTransformGround_;
	std::unique_ptr<Model> modelBunny_;
	std::unique_ptr<Animations>animation_;

	// 球
	std::unique_ptr<Sphere> sphere_;
	std::unique_ptr<Model> modelGround_;

	// 速度
	Vector3 velocity_ = {};
	// 目標の角度
	float destinationAngleY_ = 0.0f;

	//ローダー
	std::unique_ptr<ModelLoader> loader_;
	// 地面
	std::unique_ptr<Ground> ground_;
	std::unique_ptr<Skybox> skybox_;

	//プレイヤー
	std::unique_ptr<DebugPlayer> debugPlayer_;

	//PosteEffect
	Grayscale grayscale_;
	Vignetting vignetting_;
	Gaussian smoothing_;
	OutLineStyle outLine_;
	bool postEffects[4];

	float env_;
};