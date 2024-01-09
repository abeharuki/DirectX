#include "TitleScene.h"

void TitleScene::Initialize() {
	audio_ = Audio::GetInstance();
	audioData_ = audio_->SoundLoadWave("resources/audio/fanfare.wav");
	// audio_->SoundPlayWave(audioData_, true, 3.0f);

	spriteTitle_.reset(Sprite::Create("resources/Title/title.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));
	spriteRule_.reset(Sprite::Create("resources/Title/rule.png"));
	rule_ = false;
	pos_.x = 1280.0f;
}

void TitleScene::Update() {
	spriteTitle_->SetSize({3.6f, 2.0f});
	spritePushA_->SetSize({3.6f, 2.0f});
	spriteRule_->SetSize({3.6f, 2.3f});
	spriteRule_->SetPosition(pos_);
	if (KeyInput::GetInstance()->GetPadConnect()) {
		if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			rule_ = true;
		}
	}

	if (rule_) {
		const float speed = 30.0f;
		if (pos_.x > 0.0f) {
			pos_.x -= speed;
		} else {
			pos_.x = 0.0f;
			if (KeyInput::GetInstance()->GetPadConnect()) {
				if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
					sceneNo_ = GAME;
				}
			}
		}
		
	}

}

void TitleScene::Draw() {
	Transform uv;
	uv.scale = {0.0f, 0.0f, 0.0f};
	uv.rotate = {0.0f, 0.0f, 0.0f};
	uv.translate = {0.0f, 0.0f, 0.0f};
	spriteTitle_->Draw(uv);
	spritePushA_->Draw(uv);
	spriteRule_->Draw(uv);
}