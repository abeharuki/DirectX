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


class DebugScene: public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void RenderDirect() override;
	void CameraMove();
	Vector3 GetLocalPosition();
	void CheckAllCollision();
	// 当たり判定の関数
	bool IsCollidingCircleRect(float cx, float cy, float radius,
		float rx, float ry, float rw, float rh) {
		// 四角形の中心からのオフセットを計算
		float dx = std::abs(cx - rx);
		float dy = std::abs(cy - ry);

		// 円が四角形の外に完全に出ている場合
		if (dx > (rw / 2 + radius) || dy > (rh / 2 + radius)) {
			return false;
		}

		// 円の中心が四角形の中にある場合
		if (dx <= rw / 2 || dy <= rh / 2) {
			return true;
		}

		// 四角形の角との距離が円の半径以下かどうかをチェック
		float cornerDistanceSq = (dx - rw / 2) * (dx - rw / 2) +
			(dy - rh / 2) * (dy - rh / 2);

		return cornerDistanceSq <= (radius * radius);
	}
	// 四角形同士の当たり判定関数
	bool IsCollidingRectRect(float x1, float y1, float w1, float h1,
		float x2, float y2, float w2, float h2) {
		// 四角形1の右端が四角形2の左端よりも左側にある場合
		if (x1 + w1 / 2 < x2 - w2 / 2) return false;

		// 四角形1の左端が四角形2の右端よりも右側にある場合
		if (x1 - w1 / 2 > x2 + w2 / 2) return false;

		// 四角形1の上端が四角形2の下端よりも下側にある場合
		if (y1 + h1 / 2 < y2 - h2 / 2) return false;

		// 四角形1の下端が四角形2の上端よりも上側にある場合
		if (y1 - h1 / 2 > y2 + h2 / 2) return false;

		// どれにも該当しなければ重なっている
		return true;
	}
private:
	// 光の数値
	DirectionLight directionLight_{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -2.3f, -1.1f},
		1.0f,
		true,
	};

	WorldTransform worldTransform_;
	WorldTransform worldTransformSkybox_;
	WorldTransform worldTransformAnimation_;
	WorldTransform worldTransformModel_;

	//ローダー
	std::unique_ptr<ModelLoader> loader_;
	ViewProjection viewProjection_;

	std::unique_ptr<Stage> stage_;
	std::unique_ptr<DebugPlayer> debugPlayer_;
	std::unique_ptr<DebugEnemy> debugEnemy_;
	std::unique_ptr<Skybox> skybox_;
	std::unique_ptr<Model> model_;
	std::unique_ptr<Sprite> backGroundSprite_[5];
	float a_;
	bool impactScale_;

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;
	Vector3 impactVelocity_;
	Vector3 impactPos_;
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<Animations>animation_;
	int AnimationNum_ = 0;
	float animaflame_ = 0.0f;

	//パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_;
	int particleCount_;
	bool particleFlag_;

	// 速度
	Vector3 velocity_ = {};
	// 目標の角度
	float destinationAngleY_ = 0.0f;

	
	

	//アニメーションdissolve
	DissolveStyle animeDissolve_;
	bool isAnimeDissolve_ = false;

	//PosteEffect
	Grayscale grayscale_;
	Vignetting vignetting_;
	Gaussian smoothing_;
	OutLineStyle outLine_;
	RadialBlur radialBlur_;
	DissolveStyle dissolve_;
	Random random_;
	int32_t isBlur_;
	Vector2 gasianBlur_ = {0.001f,0.001f};
	BloomStyle bloom_;
	HSVMaterial hsv_;
	bool postEffects[9];

	float env_;

	

};