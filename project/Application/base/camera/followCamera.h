#pragma once
#include "Input.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "math.h"
#include <imgui.h>
#include <memory>

//カメラクラスの定数
namespace CameraConstants {
	// カメラ移動に関する設定
	const Vector3 kEnemyMoveTarget = { 0.0f,6.0f,-15.0f };  // 敵位置の座標
	
	const Vector3 kPlayerMoveTarget = { 3.f,4.f,-50.0f };  // プレイヤー位置の座標
	
	const float kMoveLerpRateToEnemy = 0.05f;  // 敵への移動の補間速度
	const float kMoveLerpRateToPlayer = 0.08f;  // プレイヤーへの移動の補間速度

	const float kMoveThresholdX = -0.1f;  // X座標の閾値
	
}

/**
 * @file followCamera
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

	//タイトルシーン時のカメラの更新
	void TitleUpdate();

	//カメラのターゲットの設定
	void SetTarget(const WorldTransform* target);

	//カメラ座標のゲッター
	const ViewProjection& GetViewProjection() { return viewProjection_; }

	//カメラの向きのリセット
	void Reset();

	// 追従対象からのオフセット計算
	Vector3 calculateOffset() const;
	// 追従対象からのオフセット計算
	Vector3 titleCalculateOffset() const;

	Vector3 GetTargetWordPos();

	// 調整項目の適用
	void ApplyGlobalVariables();

	//アングルの初期化
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

	//敵をターゲットにしているかのフラグ受け取り
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