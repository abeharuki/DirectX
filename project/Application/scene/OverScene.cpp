#include "Framework/SceneManager.h"
#include "OverScene.h"
#include <imgui.h>

void OverScene::Initialize() {}

void OverScene::Update() {

	if (KeyInput::PushKey(DIK_P)) {
		SceneManager::GetInstance()->ChangeScene("TitleScene");
	}

	ImGui::Begin("GameOverScene");
	ImGui::Text("TitleScene : push::P");
	ImGui::End();
}

void OverScene::Draw() {}