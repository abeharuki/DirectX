#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>
#include <numbers>


GameScene::GameScene() {}

GameScene::~GameScene() {}

GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize() {
	
	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -10.0f};
	
	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(Model::CreateModelFromObj("resources/skydome/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());

	// 地面
	ground_ = std::make_unique<Ground>();
	// 3Dモデルの生成
	modelGround_.reset(Model::CreateModelFromObj("resources/ground/ground.obj", "resources/ground/ground.png"));
	ground_->Initialize(modelGround_.get());
	
	//プレイヤー
	playerManager_ = std::make_unique<PlayerManager>();
	playerManager_->Initialize();

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&playerManager_->GetWorldTransform());

	// 自キャラの生成と初期化処理
	playerManager_->SetViewProjection(&followCamera_->GetViewProjection());

	//敵
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize();

	//タンク
	tankManager_ = std::make_unique<TankManager>();
	tankManager_->Initialize();
	//レンジャー
	renjuManager_ = std::make_unique<RenjuManager>();
	renjuManager_->Initialize();
	//ヒーラー
	healerManager_ = std::make_unique<HealerManager>();
	healerManager_->Initialize();

}

void GameScene::Update() {

	
	

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = CLEAR;
	}

	
	playerManager_->Update();
	healerManager_->Update();
	renjuManager_->Update();
	tankManager_->Update();
	enemyManager_->Update();
	// 追従カメラの更新
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();
	skydome_->Update();
	ground_->Update();
	//プレイヤーに追従
	healerManager_->followPlayer(playerManager_->GetWorldPos());
	renjuManager_->followPlayer(playerManager_->GetWorldPos());
	tankManager_->followPlayer(playerManager_->GetWorldPos());
	//敵の座標の取得
	renjuManager_->SetEnemypPos(enemyManager_->GetWorldPos());
	tankManager_->SetEnemypPos(enemyManager_->GetWorldPos());
}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::LightDraw(color_,direction_, intensity_);

	// 天球
	skydome_->Draw(viewProjection_,false);
	//地面
	ground_->Draw(viewProjection_,false);
	//プレイヤー
	playerManager_->Draw(viewProjection_);
	//敵
	enemyManager_->Draw(viewProjection_);
	// タンク
	tankManager_->Draw(viewProjection_);
	// ヒーラー
	healerManager_->Draw(viewProjection_);
	// レンジャー
	renjuManager_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

//
//void GameScene::CheckAllCollision() {
//	 判定対象AとBの座標
//	Vector3 posA, posB;
//
//#pragma region 自キャラとヒーラーの当たり判定
//	 自キャラ座標
//	posA = playerManager_->GetWorldPos();
//	ヒーラー
//	posB = healerManager_->
//
//}