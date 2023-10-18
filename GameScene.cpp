#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include "externals/imgui/imgui.h"


GameScene::GameScene() {}

GameScene::~GameScene() { 
	

}

GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize() {

	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -5.0f};


	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(
	    Model::CreateModelFromObj("resources/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());
}

void GameScene::Update() {
	
	viewProjection_.UpdateMatrix();
	skydome_->Update();
}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 天球
	skydome_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	//sprite_->Draw(worldTransformA_);
	
	//sprite2_->Draw(worldTransformB_);
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}