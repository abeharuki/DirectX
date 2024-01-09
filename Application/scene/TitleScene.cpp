#include "TitleScene.h"

void TitleScene::Initialize() {
	audio_ = Audio::GetInstance();
	audioData_ = audio_->SoundLoadWave("resources/audio/fanfare.wav");
	// audio_->SoundPlayWave(audioData_, true, 3.0f);
	spriteHP0_.reset(Sprite::Create("resources/Player/life1.png"));
	spriteHPG0_.reset(Sprite::Create("resources/Player/life0.png"));

	spriteHP_.reset(Sprite::Create("resources/enemy/HP.png"));
	
}

void TitleScene::Update() {
	
	if (KeyInput::GetInstance()->GetPadConnect()) {
		if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
			sceneNo_ = GAME;
		}
	}

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = GAME;
	}
}

void TitleScene::Draw() {
	Transform uv;
	spriteHPG0_->Draw(uv);
	spriteHP0_->Draw(uv);
	spriteHP_->Draw(uv);
}