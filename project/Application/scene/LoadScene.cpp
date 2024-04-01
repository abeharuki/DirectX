#include "Framework/SceneManager.h"
#include "LoadScene.h"
#include <imgui.h>

void LoadScene::Initialize() {
	backSprite_.reset(Sprite::Create("resources/load/backGround.png"));
	nowLoadingSprite_.reset(Sprite::Create("resources/load/NowLoading.png"));
	loadSprite_.reset(Sprite::Create("resources/load/LoadSprite.png"));
	backSprite_->SetSize({ 1280.0f,720.0f });
	nowLoadingSprite_->SetPosition({ 1280/2.0f,720.0f/2.0f});
	nowLoadingSprite_->SetAnchorPoint({ 0.5f,0.5f });
	nowLoadingSprite_->SetSize({ 1000.0f,1000.0f });
	loadSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	loadSprite_->SetPosition({ 1280.0f / 2.0f, 720.0f / 2.0f });
	loadSprite_->SetSize(Vector2(100.0f, 100.0f));
	
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
	backSprite_->Draw();
	nowLoadingSprite_->Draw();
	loadSprite_->Draw();
}