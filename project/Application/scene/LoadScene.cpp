#include "Framework/SceneManager.h"
#include "LoadScene.h"
#include <imgui.h>
#include "Manager/ParticleManager.h"
#include <PostEffects/PostEffect.h>

void LoadScene::Initialize() {
	backSprite_.reset(Sprite::Create("resources/Black.png"));
	nowLoadingSprite_.reset(Sprite::Create("resources/load/NowLoading.png"));
	loadSprite_.reset(Sprite::Create("resources/load/LoadSprite.png"));
	nowLoadingSprite_->SetSize(Vector2(1280.0f, 720.0f));
	backSprite_->SetSize({ 1290.0f,720.0f });
	loadSprite_->SetAnchorPoint({ 0.5f, 0.5f });
	loadSprite_->SetPosition({ 1280.0f / 2.0f, 720.0f / 2.0f });
	loadSprite_->SetSize(Vector2(100.0f, 100.0f));
	PostEffect::GetInstance()->isOutLine(false);
	PostEffect::GetInstance()->isRadialBlur(false);
	PostEffect::GetInstance()->isBloom(false);
	spritePos_.x = 0.0f;

}

void LoadScene::Update() {
	
	spritePos_.x += 0.1f;
	
	loadSprite_->SetRotation(spritePos_.x);

	ImGui::Begin("LoadScene");
	ImGui::Text("LoadScene");
	ImGui::End();
}

void LoadScene::RenderDirect() {
	
}

void LoadScene::Draw() {
	backSprite_->Draw();
	nowLoadingSprite_->Draw();
	loadSprite_->Draw();
}