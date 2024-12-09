#include "OverScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>

void OverScene::Initialize() {
	spriteOver_.reset(Sprite::Create("resources/over.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));
	alpha_ = 1.0f;
	// フェードイン・フェードアウト用スプライト
	spriteBack_.reset(Sprite::Create("resources/Black.png"));
	spriteBack_->SetSize({ 1290.0f, 720.0f });

	isFadeIn_ = true;
	isFadeOut_ = false;
	isFede_ = false;

	PostEffect::GetInstance()->isGrayscale(true);
	PostEffect::GetInstance()->isRadialBlur(false);

	/*viewProjection_.Initialize();
	viewProjection_.rotation_.x = 0.28f;
	viewProjection_.translation_ = { 0.0f, 3.0f, -9.0f };*/
}

void OverScene::Update() {
	
	Fade();
	//viewProjection_.UpdateMatrix();
	if (isFede_) {
		SceneManager::GetInstance()->ChangeScene("TitleScene");
	}
}

void OverScene::Draw() {
	spriteOver_->Draw();
	spritePushA_->Draw();
	spriteBack_->Draw();
	
}

void OverScene::RenderDirect() {

}

void OverScene::Fade() {
	if (Input::GetInstance()->GetPadConnect()) {
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A) && !isFadeIn_) {
			isFadeOut_ = true;
		}
	}

	if (Input::PushKey(DIK_P)) {

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