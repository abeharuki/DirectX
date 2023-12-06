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
	;
	Vector3 from0 = Math::Normalize(Vector3{1.0f, 0.7f, 0.5f});
	Vector3 to0 = -from0;
	Vector3 from1 = Math::Normalize(Vector3{-0.6f, 0.9f, 0.2f});
	Vector3 to1 = Math::Normalize(Vector3{0.4f, 0.7f, -0.5f});
	Matrix4x4 rotateMatrix0 = Math::DirectionToDirection(
	    Math::Normalize(Vector3{1.0f, 0.0f, 0.0f}), Math::Normalize(Vector3{-1.0f, 0.0f, 0.0f}));
	Matrix4x4 rotateMatrix1 = Math::DirectionToDirection(from0, to0);
	Matrix4x4 rotateMatrix2 = Math::DirectionToDirection(from1, to1);
	
	Math::MatrixScreenPrintf(rotateMatrix0, "rotateMatrix0");
	Math::MatrixScreenPrintf(rotateMatrix1, "rotateMatrix1");
	Math::MatrixScreenPrintf(rotateMatrix2, "rotateMatrix2");

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