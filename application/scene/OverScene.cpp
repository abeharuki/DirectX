#include "OverScene.h"

void OverScene::Initialize() { spriteOver_.reset(Sprite::Create("resources/over.png")); }

void OverScene::Update() {

	if (KeyInput::GetInstance()->GetPadConnect()) {
		if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			sceneNo_ = GAME;
		}
	}

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = TITLE;
	}
}

void OverScene::Draw() {
	Transform uv;
	spriteOver_->Draw(uv);
}