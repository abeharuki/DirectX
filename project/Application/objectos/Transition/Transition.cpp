#include "Transition.h"
#include <Input.h>

void Transition::Initialize(){
	alpha_ = TransitionConstants::kMaxAlpha;
	// フェードイン・フェードアウト用スプライト
	spriteBack_.reset(Sprite::Create("resources/Black.png"));
	spriteBack_->SetSize(TransitionConstants::kSpriteSize);
	spriteBack_->SetSize({ 1290.0f,720.0f });
	isFadeIn_ = true;
	isFadeOut_ = false;
	isFade_ = false;
}

void Transition::Update(){
	

	// フェードの進行
	UpdateFade(alpha_);

	// スプライトの色にアルファ値を反映
	spriteBack_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });
}


void Transition::Draw(){
	spriteBack_->Draw();
}

void Transition::ChangeScene()
{
	// 入力でフェードアウトを開始
	if ((Input::GetInstance()->GetPadConnect() && Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) ||
		(Input::PushKey(DIK_P))) {
		if (!isFadeIn_) {
			isFadeOut_ = true;
		}
	}
}


void Transition::UpdateFade(float& alpha) {
	if (isFadeIn_) {
		// フェードイン処理
		if (alpha > TransitionConstants::kMinAlpha) {
			alpha -= TransitionConstants::kAlphaChangeRate;
		}
		else {
			alpha = 0.0f;
			isFadeIn_ = false;
		}
	}
	else if (isFadeOut_) {
		// フェードアウト処理
		if (alpha < TransitionConstants::kMaxAlpha) {
			alpha += TransitionConstants::kAlphaChangeRate;
		}
		else {
			alpha = TransitionConstants::kMaxAlpha;
			isFade_ = true;  // フェードアウト完了
		}
	}
}