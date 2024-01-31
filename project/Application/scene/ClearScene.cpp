#include "Framework/SceneManager.h"
#include "ClearScene.h"

void ClearScene::Initialize() {}

void ClearScene::Update() {

	if (KeyInput::PushKey(DIK_P)) {
		SceneManager::GetInstance()->ChangeScene("TitleScene");
	}
}

void ClearScene::Draw() {}