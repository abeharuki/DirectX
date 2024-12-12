#include "ClearScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>

void ClearScene::Initialize() {
	spriteClear_.reset(Sprite::Create("resources/clear.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	PostEffect::GetInstance()->isOutLine(false);
	PostEffect::GetInstance()->isRadialBlur(false);
	PostEffect::GetInstance()->isBloom(false);
}

void ClearScene::Update() {

	transition_->ChangeScene();
	transition_->Update();

	if (transition_->GetFade()) {
		SceneManager::GetInstance()->ChangeScene("TitleScene");
	}
}

void ClearScene::Draw() {
	spriteClear_->Draw();
	spritePushA_->Draw();
	transition_->Draw();
}

void ClearScene::RenderDirect() {
	
}

