#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>


GameScene::GameScene() {}

GameScene::~GameScene() {}

GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize() {
	
	
}

void GameScene::Update() {
	Vector3 axis = Math::Normalize({1.0f,1.0f,1.0f});
	float angle = 0.44f;
	Matrix4x4 rotateMatrix = Math::MakeRotateAxisAngle(axis, angle);
	Math::MatrixScreenPrintf(rotateMatrix, "rotateMatrix");

}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	//Model::LightDraw(color_,direction_, intensity_);

	
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}