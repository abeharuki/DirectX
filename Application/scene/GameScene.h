#pragma once
#include "IScene.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Sprite.h"
#include "Skydome.h"
#include "Sphere.h"
#include "Player.h"
#include "Enemy.h"
#include "camera/FollowCamera.h"
#include "Ground.h"
#include "MoveFloor.h"
#include "camera/LockOn.h"
#include <list>
#include "Particle.h"


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public IScene {
public: // メンバ関数
	void Initialize() override;
	void Update() override;
	void Draw() override;
	
	GameScene();
	~GameScene();

	static GameScene* GetInstance();
	// 当たり判定
	void CheckAllCollision();

private://基本変数
	//光の数値
	Vector4 lightColor_ = {1.0f,1.0f,1.0f,1.0};
	Vector3 direction_ = {0.0f,-2.0f,0.0f};
	float intensity_ = 1.0f;

private: // メンバ変数
	WorldTransform worldTransform_;
	WorldTransform worldTransformParticle_;
	ViewProjection viewProjection_;

	// player
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> modelPlayer_;
	std::unique_ptr<Model> modelHammer_;

	// Enemy
	std::list<std::unique_ptr<Enemy>> enemies_;
	std::unique_ptr<Model> modelEnemy_;

	// 板ポリ
	std::unique_ptr<Particle> particle_;

	// 天球
	std::unique_ptr<Skydome> skydome_;
	// 天球3Dモデル
	std::unique_ptr<Model> modelSkydome_;
	// 地面
	std::unique_ptr<Ground> ground_;
	// 地面3Dモデル
	std::unique_ptr<Model> modelGround_;
	// 床
	std::unique_ptr<MoveFloor> floor_;
	// 床3Dモデル
	std::unique_ptr<Model> modelFloor_;
	// レールカメラ
	std::unique_ptr<FollowCamera> followCamera_;
	// ロックオン
	std::unique_ptr<LockOn> lockOn_;
	std::unique_ptr<Sprite> lockOnMark_;
	bool collision1_ = false;
	bool collision2_ = false;

	Emitter emitter_;
	AccelerationField field_;

	Transform transform_;

	Vector3 enemyPos[5] = {
	    {60.0f,   0.0f,-5.0f },
        {50.0f,  0.0f, 0.0f},
        {50.0f, 0.0f, -10.0f},
	    {-50.0f,  0.0f, 10.0f  },
        {-50.0f, 0.0f, -10.0f  },
	};

};
