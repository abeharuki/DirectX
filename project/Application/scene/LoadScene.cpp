#include "Framework/SceneManager.h"
#include "LoadScene.h"
#include <imgui.h>

void LoadScene::Initialize() {
	sprite_.reset(Sprite::Create("resources/uvChecker.png"));
	uv.scale = { 0.0f, 0.0f, 0.0f };
	uv.rotate = { 0.0f, 0.0f, 0.0f };
	uv.translate = { 0.0f, 0.0f };
	spritePos_.x = 0.0f;
}

void LoadScene::Update() {
	
	spritePos_.x += 10.0f;
	sprite_->SetPosition(spritePos_);
	//sprite_->SetSize({ 5.0f, 5.0f });

	ImGui::Begin("LoadScene");
	ImGui::Text("LoadScene");
	ImGui::End();
}

void LoadScene::Draw() {
	sprite_->Draw(uv);

}