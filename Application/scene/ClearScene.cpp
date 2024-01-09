#include "ClearScene.h"

void ClearScene::Initialize() { 
	spriteClear_.reset(Sprite::Create("resources/clear.png"));
	spriteClear_->SetPosition({0.0f, 0.0f});
}

void ClearScene::Update() {
	//spriteClear_->SetSize({4.0f, 2.0f});

	if (KeyInput::GetInstance()->GetPadConnect()) {
		if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			sceneNo_ = GAME;
		}
	}

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = TITLE;
	}
}

void ClearScene::Draw() {
	Transform uv;
	spriteClear_->Draw(uv);
}