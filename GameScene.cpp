#include "GameScene.h"

#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
	// 3Dモデルの生成
	// モデルの作成
	model_ = Model::GetInstance();
	model_->Initialize(DirectXCommon::GetInstance());
	
	//Spriteの描画
	sprite_ = Sprite::GetInstance();
	sprite_->Initialize();
}

void GameScene::Update() {}


void GameScene::Draw() {
	
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(Engine::GetList());

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	model_->Draw();
		

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	sprite_->Draw();
	
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}