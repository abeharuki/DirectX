#pragma once
#include "KeyInput.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "math.h"
#include <imgui.h>
#include <memory>

// 前方宣言
class LockOn;

class FollowCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void SetTarget(const WorldTransform* target);

	const ViewProjection& GetViewProjection() { return viewProjection_; }

	void Reset();

	// 追従対象からのオフセット計算
	Vector3 calculateOffset() const;

	Vector3 GetTargetWordPos();

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	// 調整項目の適用
	void ApplyGlobalVariables();

private:
	ViewProjection viewProjection_;
	const WorldTransform* target_ = nullptr;

	KeyInput* input_ = nullptr;

	// 追跡対象の残像座標
	Vector3 interTarget_ = {};
	// 目標角度
	float destinationAngleY_ = 0.0f;
	float destinationAngleX_ = 0.0f;
	// 遅延量
	float delayAmount_ = 0.2f;

	// ロックオン
	const LockOn* lockOn_ = nullptr;
};