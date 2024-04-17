#pragma once
#include "Engine.h"
#include "Math/math.h"
#include "Math/Quaternion.h"
#include "Mesh.h"

struct ConstBufferDataWorldTransform {
	Matrix4x4 matWorld; // ローカル → ワールド変換行列
	Matrix4x4 WorldInverseTranspose;//worldの逆行列
};
struct WorldTransform {
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// マッピング済みアドレス
	ConstBufferDataWorldTransform* constMap = nullptr;
	// ローカルスケール
	Vector3 scale = {1.0f, 1.0f, 1.0f};
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotate = {0.0f, 0.0f, 0.0f};
	// ローカル座標
	Vector3 translate = {0.0f, 0.0f, 0.0f};
	//クォータニオン
	Quaternion quaternion_ = { 0.0f,0.0f,0.0f,1.0f };
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld_;
	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

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
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();

	void UpdateMatrix();
	//クォータニオンの更新
	void UpdateMatrixQuaternion();

	Vector3 GetWorldPos();
};
