#include "FollowCamera.h"

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
		interTarget_ = target_->translate;
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

void FollowCamera::Initialize() {

	utility_ = std::make_unique<Utility>();
	viewProjection_.Initialize();
}

void FollowCamera::Update() {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ジョイスティックの状態取得
	if (KeyInput::GetInstance()->GetJoystickState(0, joyState)) {
		// 回転速度
		float kCharacterSpeed = 0.05f;

		destinationAngleY_ += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kCharacterSpeed;
		destinationAngleX_ += (float)joyState.Gamepad.sThumbRY / SHRT_MAX * kCharacterSpeed / 5;
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			destinationAngleY_ = target_->rotate.y;
			destinationAngleX_ = target_->rotate.x;
		}
	}

	viewProjection_.rotation_.y =
	    Math::LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, 0.1f);
	viewProjection_.rotation_.x =
	    Math::LerpShortAngle(viewProjection_.rotation_.x, destinationAngleX_, 0.1f);

	// 追従対象がいれば
	if (target_) {
		// 追従座標の補間
		interTarget_ = Math::Lerp(interTarget_, target_->translate, 0.2f);
	}
	// 追跡対象からカメラまでのオフセット
	Vector3 offset = calculateOffset();

	// 座標をコピーしてオフセット分ずらす
	viewProjection_.translation_ = Math::Add(interTarget_, offset);

	// ビュー行列の更新
	viewProjection_.UpdateMatrix();
}