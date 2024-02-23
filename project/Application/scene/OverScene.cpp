#include "OverScene.h"
#include "Framework/SceneManager.h"

void OverScene::Initialize() {
	spriteOver_.reset(Sprite::Create("resources/over.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));

	alpha_ = 1.0f;
	// フェードイン・フェードアウト用スプライト
	spriteBack_.reset(Sprite::Create("resources/Player/B.png"));
	spriteBack_->SetSize({ 10.0f, 10.0f });

	isFadeIn_ = true;
	isFadeOut_ = false;
	isFede_ = false;
}

void OverScene::Update() {
	spriteOver_->SetSize({ 3.6f, 2.0f });
	spritePushA_->SetSize({ 3.6f, 2.0f });
	Fade();
	if (Input::GetInstance()->GetPadConnect()) {
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A) && !isFadeIn_) {
			//sceneManager_->ChangeScene("TitleScene");
			isFadeOut_ = true;
		}
	}

	if (Input::PushKey(DIK_P)) {
		//sceneManager_->ChangeScene("TiTleScene");
		isFadeOut_ = true;
	}

	if (isFede_) {
		sceneManager_->ChangeScene("TitleScene");
	}
}

void OverScene::Draw() {
	Transform uv;
	uv.scale = { 0.0f, 0.0f, 0.0f };
	uv.rotate = { 0.0f, 0.0f, 0.0f };
	uv.translate = { 0.0f, 0.0f, 0.0f };
	spriteOver_->Draw(uv);
	spritePushA_->Draw(uv);
	spriteBack_->Draw(uv);
}

void OverScene::Fade() {
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