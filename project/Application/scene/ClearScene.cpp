#include "Framework/SceneManager.h"
#include "ClearScene.h"
#include <imgui.h>

void ClearScene::Initialize() {}

void ClearScene::Update() {

	if (KeyInput::PushKey(DIK_P)) {
		SceneManager::GetInstance()->ChangeScene("TitleScene");
	}


	ImGui::Begin("GameClearScene");
	ImGui::Text("TitleScene : push::P");
	ImGui::End();
}

void ClearScene::Draw() {}