#include "GameScene.h"

#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
	
	dxCommon_ = DirectXCommon::GetInstance();
	// 3Dモデルの生成
	// モデルの作成
	model_ = Model::GetInstance();
	model_->Initializ(dxCommon_);
	
}

void GameScene::Update() {}


void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	
	#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	model_->Draw();
		

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

}