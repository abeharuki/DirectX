#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include "externals/imgui/imgui.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	worldTransformBase_.Initialize();
	worldTransformB_.Initialize();

	// 3Dモデルの生成
	// モデルの作成
	model_ = new Model;
	model_ = Model::CreateModelFromObj("resources", "axis.obj", "resources/uvChecker.png");
	
	
	
	
	//Spriteの描画
	
	
	sprite2_ = std::make_unique<Sprite>();
	sprite2_->Create("resources/uvChecker.png");

	sprite_ = std::make_unique<Sprite>();
	sprite_->Create("resources/monsterBall.png");


}

void GameScene::Update() {
	
	
	
	worldTransformBase_.UpdateMatrix();
	worldTransformB_.UpdateMatrix();

	ImGui::Begin("Sprite");
	ImGui::DragFloat3("Sprite1", &worldTransformBase_.translate.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("Sprite2", &worldTransformB_.translate.x, 1.0f, 0.0f, 1280.0f);

	ImGui::DragFloat2("Sprite1", &pos1.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("Sprite2", &pos2.x, 1.0f, 0.0f, 1280.0f);
	ImGui::End();

}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw();

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
	sprite2_->Draw(worldTransformBase_);
	sprite_->Draw(worldTransformB_);
	
	
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}