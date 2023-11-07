#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>


GameScene::GameScene() {}

GameScene::~GameScene() { 
	

}

GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize() {
	
	
}

void GameScene::Update() {

	
}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	
	
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