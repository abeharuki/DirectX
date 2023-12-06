#include "TitleScene.h"
#include <imgui.h>

void TitleScene::Initialize() {}

void TitleScene::Update() {

	Quaternion q1 = {2.0f, 3.0f, 4.0f, 1.0f};
	Quaternion q2 = {1.0f, 3.0f, 5.0f, 2.0f};
	Quaternion identity = Math::IdentityQuaternion();
	Quaternion conj = Math::Conjugate(q1);
	Quaternion inv = Math::Inverse(q1);
	Quaternion normal = Math::Normalize(q1);
	Quaternion mul1 = Math::Multiply(q1, q2);
	Quaternion mul2 = Math::Multiply(q2, q1);
	float norm = Math::Norm(q1);

	Math::MatrixScreenPrintf(identity, "Identity");
	Math::MatrixScreenPrintf(conj, "conj");
	Math::MatrixScreenPrintf(inv, "inv");
	Math::MatrixScreenPrintf(normal, "normal");
	Math::MatrixScreenPrintf(mul1, "Multiply(q1, q2)");
	Math::MatrixScreenPrintf(mul2, "Multiply(q2, q1)");
	
	ImGui::Begin("Norm");
	ImGui::Text("%1.2f", norm);
	ImGui::End();

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = GAME;
	}
}

void TitleScene::Draw() {}