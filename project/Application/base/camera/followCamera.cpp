#include "FollowCamera.h"
#include <GlobalVariables.h>

void FollowCamera::Initialize() {

	viewProjection_.Initialize();
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	globalVariables->AddItem(groupName, "delayAmount", delayAmount_);

#endif
}

void FollowCamera::Update() {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ジョイスティックの状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 回転速度
		float kCharacterSpeed = 0.05f;

		destinationAngleY_ += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kCharacterSpeed;
		destinationAngleX_ -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * kCharacterSpeed / 5;
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			destinationAngleY_ = target_->rotate.y;
			destinationAngleX_ = target_->rotate.x;
		}
	}

	if (Input::PressKey(DIK_UPARROW)) {
		destinationAngleX_ -= 0.1f;
	} else if (Input::PressKey(DIK_DOWNARROW)) {
		destinationAngleX_ += 0.1f;
	}

	if (Input::PressKey(DIK_LEFTARROW)) {
		destinationAngleY_ -= 0.1f;
	} else if (Input::PressKey(DIK_RIGHTARROW)) {
		destinationAngleY_ += 0.1f;
	}

	viewProjection_.rotation_.y =
	    Math::LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, 0.1f);
	viewProjection_.rotation_.x =
	    Math::LerpShortAngle(viewProjection_.rotation_.x, destinationAngleX_, 0.1f);

	// 追従対象がいれば
	if (target_) {
		// 追従座標の補間
		interTarget_ = Math::Lerp(interTarget_, GetTargetWordPos(), delayAmount_);
	}
	// 追跡対象からカメラまでのオフセット
	Vector3 offset = calculateOffset();

	// 座標をコピーしてオフセット分ずらす
	viewProjection_.translation_ = Math::Add(interTarget_, offset);

	// ビュー行列の更新
	viewProjection_.UpdateMatrix();
	ApplyGlobalVariables();
}

Vector3 FollowCamera::calculateOffset() const {
	// 追従対象からのオフセット
	Vector3 offset = {0.0f, 4.0f, -15.0f};

	Matrix4x4 rotateMatrix = Math::Multiply(
	    Math::MakeRotateXMatrix(viewProjection_.rotation_.x),
	    Math::Multiply(
	        Math::MakeRotateYMatrix(viewProjection_.rotation_.y),
	        Math::MakeRotateZMatrix(viewProjection_.rotation_.z)));

	offset = Math::TransformNormal(offset, rotateMatrix);

	return offset;
}

void FollowCamera::Reset() {
	// 追従対象がいれば
	if (target_) {
		// 追従座標・角度初期化
		interTarget_ = GetTargetWordPos();
		viewProjection_.rotation_.y = target_->rotate.y;
	}

	destinationAngleY_ = viewProjection_.rotation_.y;

	// 追従対象からのオフセット
	Vector3 offset = calculateOffset();
	viewProjection_.translation_ = Math::Add(interTarget_, offset);
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	Reset();
}

Vector3 FollowCamera::GetTargetWordPos() {
	Vector3 result;
	result.x = target_->matWorld_.m[3][0];
	result.y = target_->matWorld_.m[3][1];
	result.z = target_->matWorld_.m[3][2];
	return result;
}

void FollowCamera::ApplyGlobalVariables() {
#ifdef _DEBUG

	GlobalVariables* globalVaribles = GlobalVariables::GetInstance();
	const char* groupName = "FollowCamera";
	delayAmount_ = globalVaribles->GetFloatValue(groupName, "delayAmount");

#endif
}