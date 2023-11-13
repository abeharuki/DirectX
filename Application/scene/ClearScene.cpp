#include "ClearScene.h"

void ClearScene::Initialize() {}

void ClearScene::Update() {

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = TITLE;
	}
}

void ClearScene::Draw() {}