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
	inputHandler_ = std::make_unique<InputHandler>();
	inputHandler_->AssingMoveRightCommand2PressKeyD();
	inputHandler_->AssingMoveLeftCommand2PressKeyA();

	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -5.0f};

	// プレイヤー
	modelPlayer_.reset(Model::CreateModelFromObj("resources/cube/cube.obj", "resources/cube/cube.jpg"));
	player_ = std::make_unique<Player>();
	std::vector<Model*> playerModels = {modelPlayer_.get()};
	player_->Initialize(playerModels);


	
}

void GameScene::Update() { 
	command_ = inputHandler_->HandleInput();

	if (this->command_) {
		command_->Exec(*player_);
	}

	player_->Update();
}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	//Model::LightDraw(color_,direction_, intensity_);

	// プレーヤー
	player_->Draw(viewProjection_, false);
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}