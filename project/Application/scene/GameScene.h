#pragma once
#include "Framework/IScene.h"
#include "Input.h"
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
#include "CollisionManager/CollisionManager.h"
#include <Command/Command.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public IScene {
public: // メンバ関数
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void RenderDirect() override;

	GameScene();
	~GameScene();

	//static GameScene* GetInstance();
	void CheckAllCollision();

	void Fade();

private://基本変数
	// 光の数値
	DirectionLight directionLight_{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -2.0f, 0.0f},
		1.0f,
		true,
	};

private: // メンバ変数

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;
	ViewProjection viewProjection_;

	// 天球
	std::unique_ptr<Skydome> skydome_;
	// 天球3Dモデル
	std::unique_ptr<Model> modelSkydome_;
	// 地面
	std::unique_ptr<Ground> ground_;
	// 地面3Dモデル
	std::unique_ptr<Model> modelGround_;

	//フェードイン・フェードアウト用スプライト
	std::unique_ptr<Sprite> spriteBack_;

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

	//コマンド
	std::unique_ptr<Command> command_;

	bool isFadeOut_;
	bool isFadeIn_;
	float alpha_;

};