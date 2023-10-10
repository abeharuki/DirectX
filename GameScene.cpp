#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include "externals/imgui/imgui.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
	// 3Dモデルの生成
	// モデルの作成
	model_ = std::make_unique<Model>();
	model_->Initialize(DirectXCommon::GetInstance());
	
	model2_ = std::make_unique<Model>();
	model2_->Initialize(DirectXCommon::GetInstance());
	
	
	//Spriteの描画
	
	
	sprite2_ = std::make_unique<Sprite>();
	sprite2_->Create("resources/uvChecker.png", {0,100});

	sprite_ = std::make_unique<Sprite>();
	sprite_->Create("resources/monsterBall.png", pos1);
}

void GameScene::Update() {

	sprite_->SetPosition(pos1);
	//sprite2_->SetPosition(pos2);

	ImGui::Begin("Sprite");
	ImGui::DragFloat2("Sprite1", &pos1.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("Sprite2", &pos2.x, 1.0f, 0.0f, 1280.0f);
	ImGui::End();

}


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
	
	sprite2_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}