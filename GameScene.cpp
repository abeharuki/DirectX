#include "GameScene.h"

#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	dxCommon_ = DirectXCommon::GetInstance();
	dxCommon_->Initialize(win, 1280, 720);
	
}

void GameScene::Update() {}


void GameScene::Draw() {

	// コマンドリストの取得
	
	
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	//Model::PostDraw();

}