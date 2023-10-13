#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include "externals/imgui/imgui.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize() {

	

	// 3Dモデルの生成
	// モデルの作成
	model_ = std::make_unique<Model>();
	model_->Initialize("resources/axis.obj", "resources/uvChecker.png");
	//model_.reset(Model::CreateModelFromObj("resources/axis.obj","resources/uvChecker.png"));
	
	
	
	//Spriteの描画
	
	
	sprite2_ = std::make_unique<Sprite>();
	sprite2_->Create("resources/uvChecker.png");

	sprite_ = std::make_unique<Sprite>();
	sprite_->Create("resources/monsterBall.png");

	worldTransformBase_.Initialize();
	worldTransformA_.Initialize();
	worldTransformB_.Initialize();
}

void GameScene::Update() {
	
	
	
	worldTransformBase_.UpdateMatrix();
	

	ImGui::Begin("OBJ");
	ImGui::DragFloat3("OBJ", &worldTransformBase_.translate.x, 1.0f, -100.0f, 100.0f);
	ImGui::End();


	ImGui::Begin("Sprite");
	ImGui::DragFloat3("SA", &worldTransformA_.translate.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("SAR", &worldTransformA_.rotate.x, 1.0f, 0.0f, 1280.0f);

	ImGui::DragFloat3("SB", &worldTransformB_.translate.x, 1.0f, 0.0f, 1280.0f);
	ImGui::DragFloat2("SBR", &worldTransformB_.rotate.x, 1.0f, 0.0f, 1280.0f);
	ImGui::End();

}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	model_->Draw(worldTransformBase_,model_.get());
	

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	Sprite::Draw(worldTransformA_, sprite_.get());
	
	//Sprite::Draw(worldTransformB_, sprite2_.get());
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}