#include "OverScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>

void OverScene::Initialize() {
	spriteOver_.reset(Sprite::Create("resources/over.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));

	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	PostEffect::GetInstance()->isGrayscale(true);
	PostEffect::GetInstance()->isRadialBlur(false);

	
}

void OverScene::Update() {
	transition_->ChangeScene();
	transition_->Update();
	
	if (transition_->GetFade()) {
		SceneManager::GetInstance()->ChangeScene("TitleScene");
	}
}

void OverScene::Draw() {
	spriteOver_->Draw();
	spritePushA_->Draw();
	transition_->Draw();
}

void OverScene::RenderDirect() {

}

