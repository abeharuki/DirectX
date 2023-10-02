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
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	
	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	
	// スプライト描画後処理
	
	
	// 深度バッファクリア
	// 3Dオブジェクト描画後処理
	//Model::PostDraw();

}