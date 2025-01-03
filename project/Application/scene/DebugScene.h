#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include "3d/ModelLoader.h"
#include <camera/followCamera.h>
#include <PostEffects/PostEffect.h>
#include <Skybox.h>
#include <Skydome.h>
#include <Ground.h>
#include <Sphere.h>
#include "3d/LineBox.h"
#include "CollisionManager/CollisionManager.h"
#include <ParticleSystem.h>
#include <Enemy/EnemyManager.h>
#include "CollisionManager/CollisionConfig.h"
#include "Manager/ParticleManager.h"
#include <Sprite.h>

/**
 * @file DebugScene
 * @brief デバックをするためのクラス
 */
class DebugScene: public IScene {
public:
	//シーンの初期化
	void Initialize() override;
	//フレームごとの更新処理
	void Update() override;
	//シーンの描画
	void Draw() override;
	//ポストエフェクトのかからないオブジェクトの描画
	void RenderDirect() override;
	//カメラの動き処理
	void CameraMove();
	//カメラを動かすときの座標
	Vector3 GetLocalPosition();
	//当たり判定のチェック
	void CheckAllCollision();
private:
	// 光の数値
	DirectionLight directionLight_{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -2.0f, 0.0f},
		1.0f,
		true,
	};

	WorldTransform worldTransform_;
	WorldTransform worldTransformSkybox_;
	WorldTransform worldTransformAnimation_;
	WorldTransform worldTransformCollider1_;
	WorldTransform worldTransformCollider2_;
	WorldTransform worldTransformCollider3_;
	WorldTransform worldTransformModel_;
	WorldTransform worldTransformSter_[2];
	
	//ローダー
	std::unique_ptr<ModelLoader> loader_;
	ViewProjection viewProjection_;

	Animations* animation_;
	std::unique_ptr<Skybox> skybox_;
	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> ster_[2];
	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Sphere> sphereModel_;
	float a_;
	bool impactScale_;

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;
	std::unique_ptr<ColliderManager> colliderManager_[3] = {};
	Vector3 impactVelocity_;
	Vector3 impactPos_;
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	//std::unique_ptr<Animations>animation_;
	int AnimationNum_ = 0;
	float animaflame_ = 0.0f;
	bool animationStop_ = false;

	//パーティクル
	ParticleSystem* particle_;
	EmitterSphere emitter_;
	AccelerationField accelerationFiled_;

	GravityField gravityFiled_;
	int particleCount_;
	bool particleFlag_;
	bool scaleFlag_;

	// 速度
	Vector3 velocity_ = {};
	// 目標の角度
	float destinationAngleY_ = 0.0f;

	
	float angle_[3];

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
	Transform maskUV_;
	Vector4 modelColor_ = { 1.f,1.f,1.f,1.f };
	BlendMode blendMode_ = BlendMode::kNormal;
	int blendNum_;
	bool shockwaveflag_;

	Editor::NodeEditor editor_;
};