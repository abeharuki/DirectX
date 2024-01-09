#include "TitleScene.h"

void TitleScene::Initialize() { 
	audio_ = Audio::GetInstance();
	audioData_ = audio_->SoundLoadWave("resources/audio/fanfare.wav");
	//audio_->SoundPlayWave(audioData_, true, 3.0f);
	sprite_.reset(Sprite::Create("resources/clear.png"));
	size = {1.0f, 1.0f};
}

void TitleScene::Update() {
	
	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = GAME;
	}

	sprite_->SetSize(size);
	ImGui::Begin("scene");
	ImGui::SliderFloat2("size",&size.x,0.0f,10.0f);
	ImGui::End();
}

void TitleScene::Draw() {
	Transform uv;
	uv.scale = {0.0f, 0.0f, 0.0f};
	uv.rotate = {0.0f, 0.0f, 0.0f};
	uv.translate = {0.0f, 0.0f};
	ImGui::Begin("scene");
	ImGui::SliderFloat3("s", &uv.scale.x, 0.0f, 10.0f);
	ImGui::End();
	sprite_->Draw(uv);
}