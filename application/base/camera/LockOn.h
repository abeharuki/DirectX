#pragma once
#include "KeyInput.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "math.h"
#include <Enemy.h>
#include <imgui.h>
#include <memory>

class LockOn {
public:
	void Initialize();

	void Update(
	    const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);

	void Draw();

	// 範囲外判定
	bool IsOutOfRange(
	    const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);

	// 検索
	void searchTarget(
	    const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection);

	// ワールドからスクリーン座標系に変換
	Vector3 WorldToScreen(const ViewProjection& viewProjection, const Vector3& vector);

	Vector3 GetTargetPos() const;

	bool ExistTarget() const { return target_ ? true : false; }

private:
	Transform transform_;

	KeyInput* input_ = nullptr;

	std::unique_ptr<Sprite> lockOnMark_;

	// ロックオン対象
	const Enemy* target_ = nullptr;

	const float kDegreeToRadian = 3.1415f / 180;

	// 最小距離
	float minDistance_ = 10.0f;
	// 最大距離
	float maxDistance_ = 40.0f;
	// 角度範囲
	float angleRenge_ = 20.0f * kDegreeToRadian;

	bool autoLockOn = false;
};
