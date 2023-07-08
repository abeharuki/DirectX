#pragma once
#include <cassert>
#include <format>

struct Vector2 final {
	float x;
	float y;
};

struct Vector3 final {
	float x;
	float y;
	float z;
};

struct Vector4 final {
	float x;
	float y;
	float z;
	float w;
};

struct Matrix3x3 final {
	float m[3][3];
};

struct Matrix4x4 final {
	float m[4][4];
};

class Math {
public:

	
	Matrix4x4 MakeIdentity4x4();

	// 回転X
	Matrix4x4 MakeRotateXMatrix(float theta = 0);

	// Y
	Matrix4x4 MakeRotateYMatrix(float theta = 0);

	// Z
	Matrix4x4 MakeRotateZMatrix(float theta = 0);

	// スカラー倍
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	// アフィン変換
	Matrix4x4
	    MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	// 透視投影行列
	Matrix4x4
	    MakePerspectiverFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	// 正射影行列
	Matrix4x4 MakeOrthographicMatrix(
	    float left, float top, float right, float bottom, float nearClip, float farClip);

	// ビューポート行列
	Matrix4x4 MakeViewportMatrix(
	    float left, float top, float width, float height, float minDepth, float maxDepth);

	// 逆行列
	Matrix4x4 Inverse(const Matrix4x4& m);

};