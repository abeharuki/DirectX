#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "DirectXCommon.h"
#include "Engine.h"
#include "Skydome.h"
#include <Player/PlayerManager.h>
#include "camera/followCamera.h"
#include <Enemy/EnemyManager.h>
#include <Healer/HealerManager.h>
#include <Tank/TankManager.h>
#include <Renju/RenjuManager.h>
#include "CollisionManager/CollisionManager.h"
#include <Command/Command.h>
#include <ModelLoader.h>
#include "BaseCharacter.h"

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
	

	
private:
	//シーン遷移
	void Fade();

	//バトル開始の演出
	void BattleBegin();

	//当たり判定
	void CheckAllCollision();
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
	//ローダー
	std::unique_ptr<ModelLoader> loader_;

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

	//フェーズイン・フェードアウト
	bool isFadeOut_;
	bool isFadeIn_;
	float alpha_;

	//ラジアルブラー
	RadialBlur radialBlur_;
	//戦闘移行演出
	bool battle_ = false;
	//バトル開始時のカメラ演出
	bool cameraDirection_ = false;
	int cameraDirectionTime_;


	std::vector<BaseCharacter*> characters;

};