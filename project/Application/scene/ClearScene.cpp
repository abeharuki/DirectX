#include "ClearScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>

void ClearScene::Initialize() {
	spriteClear_.reset(Sprite::Create("resources/clear.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));
	alpha_ = 1.0f;
	// フェードイン・フェードアウト用スプライト
	spriteBack_.reset(Sprite::Create("resources/Black.png"));
	spriteBack_->SetSize({ 1280.0f, 720.0f });

	isFadeIn_ = true;
	isFadeOut_ = false;
	isFede_ = false;

	PostEffect::GetInstance()->isOutLine(false);
	PostEffect::GetInstance()->isRadialBlur(false);
}

void ClearScene::Update() {

	Fade();


	if (isFede_) {
		SceneManager::GetInstance()->ChangeScene("TitleScene");
	}
}

void ClearScene::Draw() {
	spriteClear_->Draw();
	spritePushA_->Draw();
	spriteBack_->Draw();
}

void ClearScene::RenderDirect() {

}

void ClearScene::Fade() {
	if (Input::GetInstance()->GetPadConnect()) {
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A) && !isFadeIn_) {
			isFadeOut_ = true;
		}
	}

	if (Input::PushKey(DIK_P) && !isFadeIn_) {
		isFadeOut_ = true;

	}

	if (isFadeIn_) {
		if (alpha_ > 0.001f) {
			alpha_ -= 0.02f;
		}
		else {
			alpha_ = 0.0f;
			isFadeIn_ = false;
		}
	}

	if (isFadeOut_) {
		if (alpha_ < 1) {
			alpha_ += 0.02f;
		}
		else {
			alpha_ = 1.0f;
			isFede_ = true;
		}
	}
	spriteBack_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });
}