#include "TitleScene.h"

void TitleScene::Initialize() { ; }

void TitleScene::Update() {

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = GAME;
	}
}

void TitleScene::Draw() {}