#include "TitleScene.h"

void TitleScene::Initialize() { ; }

void TitleScene::Update() {

	if (KeyInput::GetPadConnect()) {
		if (KeyInput::GetPadButtonDown(XINPUT_GAMEPAD_B)) {
			sceneNo_ = GAME;
		}
	}

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = GAME;
	}
}

void TitleScene::Draw() {}