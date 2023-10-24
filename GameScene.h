#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "DirectXCommon.h"
#include "Model.h"
#include "Engine.h"
#include "Sprite.h"
#include "KeyInput.h"
#include "Skydome.h"
#include "Player.h"
#include "FollowCamera.h"
#include "Ground.h"
#include "MoveFloor.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	static GameScene* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// 当たり判定
	void CheckAllCollision();
	
private: // メンバ変数
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	//player
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> modelPlayer_;


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

	bool collision1_ = false;
	bool collision2_ = false;
};
