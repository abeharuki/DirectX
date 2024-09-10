#pragma once
#include "Engine.h"
#include "Math/math.h"
#include <d3d12.h>
#include <wrl.h>

// 定数バッファ用データ構造体
struct ConstBufferDataViewProjection {
	Matrix4x4 view;       // ワールド → ビュー変換行列
	Matrix4x4 projection; // ビュー → プロジェクション変換行列
	Vector3 cameraPos;    // カメラ座標（ワールド座標）
};

struct ViewProjection {
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// マッピング済みアドレス
	ConstBufferDataViewProjection *constMap = nullptr;

#pragma region ビュー行列の設定
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0, 0, 0 };
	// ローカル座標
	Vector3 translation_ = { 0, 0, -50 };
	//ワールド座標
	Vector3 worldPos_ = {};
#pragma endregion

#pragma region 射影行列の設定
	// 垂直方向視野角
	float fovAngleY = 45.0f * 3.141592654f / 180.0f;
	// ビューポートのアスペクト比
	float aspectRatio = (float)16 / 9;
	// 深度限界（手前側）
	float nearZ = 0.1f;
	// 深度限界（奥側）
	float farZ = 1000.0f;
#pragma endregion

	// ビュー行列
	Matrix4x4 matView;
	// 射影行列
	Matrix4x4 matProjection;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();
	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();
	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();
	/// <summary>
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();
	/// <summary>
	/// ビュー行列を更新する
	/// </summary>
	void UpdateViewMatrix();
	/// <summary>
	/// 射影行列を更新する
	/// </summary>
	void UpdateProjectionMatrix();


	/// @brief スクリーン座標から3D座標に変換する
	/// @param screenPos 2次元ベクトル
	/// @return 始点と終点座標 { 近, 遠 }
	std::pair<Vector3, Vector3> ScreenToWorld(const Vector2 &screenPos) const;


	/// @brief 3D座標からスクリーン座標に変換する
	/// @param worldPos 3次元の座標
	/// @param matVPVp 座標変換行列
	/// @return スクリーン上の座標
	Vector3 WorldToScreen(const Vector3 &worldPos) const;


private:

	/// @brief スクリーン座標から3D座標に変換する
	/// @param screenPos 2次元ベクトル
	/// @param matVPVp 座標変換行列
	/// @return 始点と終点座標 { 近, 遠 }
	static std::pair<Vector3, Vector3> ScreenToWorld(const Vector2 &screenPos, const Matrix4x4 &matVPVp);

	/// @brief 3D座標からスクリーン座標に変換する
	/// @param worldPos 3次元の座標
	/// @param matVPVp 座標変換行列
	/// @return スクリーン上の座標
	static Vector3 WorldToScreen(const Vector3 &worldPos, const Matrix4x4 &matVPVp);

	static Matrix4x4 MakeViewportMatrix(const Vector2 &LeftTop, const float &width, const float &height, const float &minDepth, const float &maxDepth);
};
