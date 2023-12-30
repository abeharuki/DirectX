#pragma once
#include "IScene.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "DirectXCommon.h"
#include "Model.h"
#include "Engine.h"
#include "Sprite.h"
#include "Skydome.h"
#include "Sphere.h"
#include "Particle.h"
#include "Ground.h"
#include <Player/PlayerManager.h>
#include "camera/followCamera.h"
#include <Enemy/EnemyManager.h>
#include <Healer/HealerManager.h>
#include <Tank/TankManager.h>
#include <Renju/RenjuManager.h>

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
	void CheckAllCollision();

private://基本変数
	//光の数値
	Vector4 color_ = {1.0f,1.0f,1.0f,1.0};
	Vector3 direction_ = {0.0f,-2.0f,0.0f};
	float intensity_ = 1.0f;

private: // メンバ変数
	ViewProjection viewProjection_;
	
	// 天球
	std::unique_ptr<Skydome> skydome_;
	// 天球3Dモデル
	std::unique_ptr<Model> modelSkydome_;
	// 地面
	std::unique_ptr<Ground> ground_;
	// 地面3Dモデル
	std::unique_ptr<Model> modelGround_;

	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;


	//プレイヤー
	std::unique_ptr<PlayerManager> playerManager_;
	// 敵
	std::unique_ptr<EnemyManager> enemyManager_;

	//タンク
	std::unique_ptr<TankManager> tankManager_;
	//ヒーラー
	std::unique_ptr<HealerManager> healerManager_;
	//レンジャー
	std::unique_ptr<RenjuManager> renjuManager_;

};
