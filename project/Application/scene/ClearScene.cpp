#include "ClearScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>

void ClearScene::Initialize() {
	spriteClear_.reset(Sprite::Create("resources/clear.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));
	spriteClear_->SetSize({ 1280.0f, 720.0f });
	spritePushA_->SetSize({ 1280.0f,720.0f });
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	
}

void ClearScene::Update() {

	transition_->ChangeScene();
	transition_->Update();

	if (transition_->GetFade()) {
		SceneManager::GetInstance()->ChangeScene("OverScene");
	}
}

void ClearScene::Draw() {
	spriteClear_->Draw();
	spritePushA_->Draw();
	transition_->Draw();
}

void ClearScene::RenderDirect() {
	
}

