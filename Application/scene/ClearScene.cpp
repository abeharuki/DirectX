#include "ClearScene.h"

void ClearScene::Initialize() {}

void ClearScene::Update() {

	if (KeyInput::GetPadConnect()) {
		if (KeyInput::GetPadButtonDown(XINPUT_GAMEPAD_B)) {
			sceneNo_ = TITLE;
		}
	}

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = TITLE;
	}
}

void ClearScene::Draw() {}