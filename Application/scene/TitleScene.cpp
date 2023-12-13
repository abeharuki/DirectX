#include "TitleScene.h"

void TitleScene::Initialize() { ; }

void TitleScene::Update() {
	Quaternion rotation =   Math::MakeRoateAxisAngleQuaternion(Math::Normalize(Vector3{1.0f, 0.4f, -0.2f}), 0.45f);
	Vector3 pointY = {2.1f, -0.9f, 1.3f};
	Matrix4x4 rotateMatrix = Math::MakeRotateMatrix(rotation);
	Vector3 rotateByQuaternion = Math::RotateVector(pointY, rotation);
	Vector3 rotateByMatrix = Math::TransformNormal(pointY, rotateMatrix);


	Math::MatrixScreenPrintf(rotation, "rotation");
	Math::MatrixScreenPrintf(rotateMatrix, "rotateMatrix ");
	Math::MatrixScreenPrintf(rotateByQuaternion, "rotateByQuaternion");
	Math::MatrixScreenPrintf(rotateByMatrix, "rotateByMatrix");

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = GAME;
	}
}

void TitleScene::Draw() {}