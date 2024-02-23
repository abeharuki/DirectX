#include "Framework/SceneManager.h"
#include "LoadScene.h"
#include <imgui.h>

void LoadScene::Initialize() {
	backSprite_.reset(Sprite::Create("resources/load/backGround.png"));
	nowLoadingSprite_.reset(Sprite::Create("resources/load/NowLoading.png"));
	loadSprite_.reset(Sprite::Create("resources/load/LoadSprite.png"));
	loadSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	loadSprite_->SetPosition({ 1280.0f / 2.0f, 720.0f / 2.0f });
	//loadSprite_.reset(Sprite::Create("resources/uvChecker.png"));
	uv.scale = { 0.0f, 0.0f, 0.0f };
	uv.rotate = { 0.0f, 0.0f, 0.0f };
	uv.translate = { 0.0f, 0.0f };
	spritePos_.x = 0.0f;
}

void LoadScene::Update() {
	
	spritePos_.x += 0.1f;
	
	loadSprite_->SetRotation(spritePos_.x);

	ImGui::Begin("LoadScene");
	ImGui::Text("LoadScene");
	ImGui::End();
}

void LoadScene::Draw() {
	backSprite_->Draw(uv);
	nowLoadingSprite_->Draw(uv);
	loadSprite_->Draw(uv);
}