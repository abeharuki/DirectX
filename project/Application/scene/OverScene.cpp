#include "Framework/SceneManager.h"
#include "OverScene.h"

void OverScene::Initialize() {}

void OverScene::Update() {

	if (KeyInput::PushKey(DIK_P)) {
		SceneManager::GetInstance()->ChangeScene("TitleScene");
	}
}

void OverScene::Draw() {}