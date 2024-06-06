#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include <Sprite.h>
#include "3d/ModelLoader.h"
#include <camera/followCamera.h>

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

	// 速度
	Vector3 velocity_ = {};
	// 目標の角度
	float destinationAngleY_ = 0.0f;

	//ローダー
	std::unique_ptr<ModelLoader> loader_;

	
};