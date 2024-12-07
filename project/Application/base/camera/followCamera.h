#pragma once
#include "Input.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "math.h"
#include <imgui.h>
#include <memory>

/**
 * @file followCamera.h
 * @brief 追従するカメラを管理するクラス
 */
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

	void InitAngle(){
		destinationAngleY_ = 0.0f;
		destinationAngleX_ = 0.0f;
		viewProjection_.translation_ = {-30.0f,6.0f,-15.0f};
		viewProjection_.rotation_ = { 0.0f,0.0f,0.0f };
		moveToEnemy_ = true;
		moveToPlayer_ = false;
		moveTime_ = 20;
		// ビュー行列の更新
		viewProjection_.UpdateMatrix();
	}

	//バトル開始時のカメラの演出
	void CameraDirection();

	//ターゲットの位置
	void SetinterTargetPos(Vector3 pos) {
		interTarget_ = pos;
	}

	bool GetMoveToEnemy() { return moveToEnemy_; }
	

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

	//どこに移動するか
	bool moveToEnemy_ = false;
	bool moveToPlayer_ = false;
	//次の目的地までの時間
	int moveTime_;
};