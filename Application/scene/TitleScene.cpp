#include "TitleScene.h"

void TitleScene::Initialize() { 
	audio_ = Audio::GetInstance();
	audioData_ = audio_->SoundLoadWave("resources/fanfare.wav");
	audio_->SoundPlayWave(audioData_, true, 3.0f);
}

void TitleScene::Update() {
	
	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = GAME;
	}
}

void TitleScene::Draw() {}