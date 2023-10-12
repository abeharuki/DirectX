#pragma once
#include "Engine.h"
#include "Math.h"
#include "Mesh.h"

struct ConstBufferDataWorldTransform {
	Matrix4x4 matWorld; // ローカル → ワールド変換行列
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
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld_;
	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

	void Initialize();

	void TransferMatrix();

	void UpdateMatrix();

	//	平行移動行列の平行移動の取得
	Vector3 GetWorldPos()const;
};
