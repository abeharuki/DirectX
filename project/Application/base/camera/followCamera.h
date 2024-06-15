#pragma once
#include "Input.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "math.h"
#include <imgui.h>
#include <memory>

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
	void TitleUpdate();

	void SetTarget(const WorldTransform* target);

	const ViewProjection& GetViewProjection() { return viewProjection_; }

	void Reset();

	// 追従対象からのオフセット計算
	Vector3 calculateOffset() const;
	// 追従対象からのオフセット計算
	Vector3 titleCalculateOffset() const;

	Vector3 GetTargetWordPos();

	// 調整項目の適用
	void ApplyGlobalVariables();

private:
	ViewProjection viewProjection_;
	const WorldTransform* target_ = nullptr;

	Input* input_ = nullptr;

	// 追跡対象の残像座標
	Vector3 interTarget_ = {};
	// 目標角度
	float destinationAngleY_ = 0.0f;
	float destinationAngleX_ = 0.0f;
	// 遅延量
	float delayAmount_ = 0.5f;
};