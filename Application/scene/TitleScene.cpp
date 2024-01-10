#include "TitleScene.h"

void TitleScene::Initialize() { ; }

void TitleScene::Update() {
	Quaternion rotation0 =   Math::MakeRoateAxisAngleQuaternion({0.71f, 0.71f, 0.0f}, 0.3f);
	Quaternion rotation1 =   Math::MakeRoateAxisAngleQuaternion({0.71f, 0.0f, 0.71f}, 3.141592f);

	Quaternion interpolate0 = Math::Slerp(rotation0,rotation1,0.0f);
	Quaternion interpolate1 = Math::Slerp(rotation0,rotation1,0.3f);
	Quaternion interpolate2 = Math::Slerp(rotation0,rotation1,0.5f);
	Quaternion interpolate3 = Math::Slerp(rotation0,rotation1,0.7f);
	Quaternion interpolate4 = Math::Slerp(rotation0,rotation1,1.0f);
	
	Math::MatrixScreenPrintf(interpolate0, "  : interpolate0, Slerp(q0, q1, 0.0f)");
	Math::MatrixScreenPrintf(interpolate1, "  : interpolate1, Slerp(q0, q1, 0.3f)");
	Math::MatrixScreenPrintf(interpolate2, "  : interpolate2, Slerp(q0, q1, 0.5f)");
	Math::MatrixScreenPrintf(interpolate3, "  : interpolate3, Slerp(q0, q1, 0.7f)");
	Math::MatrixScreenPrintf(interpolate4, "  : interpolate4, Slerp(q0, q1, 1.0f)");
	
	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = GAME;
	}
}

void TitleScene::Draw() {}