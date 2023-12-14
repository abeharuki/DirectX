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
	modelSkydome_.reset(Model::CreateModelFromObj("resources/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());

	
	
	
}

void GameScene::Update() {

	
	

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = CLEAR;
	}

	

	viewProjection_.UpdateMatrix();
	skydome_->Update();
		
}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::LightDraw(color_,direction_, intensity_);

	// 天球
	skydome_->Draw(viewProjection_,false);
	
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