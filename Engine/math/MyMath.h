#pragma once
#include <cassert>
#include <format>
#include <vector>




struct Vector2 final {
	float x;
	float y;

};

struct Vector3 final {
	float x;
	float y;
	float z;

	Vector3& operator*=(float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	Vector3& operator-=(Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3& operator+=(Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3& operator/=(float s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}
};

struct Vector4 final {
	float x;
	float y;
	float z;
	float w;
};

struct Vertex {
	Vector3 position;
	Vector4 color;
};

struct Matrix3x3 final {
	float m[3][3];
};

struct Matrix4x4 final {
	float m[4][4];
};

struct AABB {
	Vector3 min; // 最小点
	Vector3 max; // 最大点
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Sphere {
	Vector3 center; // 中心点
	float radius;   // 半径
};

struct Color {
	Vector3 rgb;
	float a;
};

struct Material {
	Color color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct MaterialData {

	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};


struct DirectionalLight {
	Vector4 color;     // ライトの色
	Vector3 direction; // ライトの向き
	float intensity;   // 輝度
};


class Math {
public:

	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

	// 加算
	static Vector3 Add(const Vector3& v1, const Vector3& v2);

	static Vector3 Multiply(float scalar, const Vector3& v);

	// 減算
	static Vector3 Subract(const Vector3& v1, const Vector3& v2);
	
	static Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);

	static Matrix4x4 MakeIdentity4x4();

	// 回転X
	static Matrix4x4 MakeRotateXMatrix(float theta = 0);

	// Y
	static Matrix4x4 MakeRotateYMatrix(float theta = 0);

	// Z
	static Matrix4x4 MakeRotateZMatrix(float theta = 0);

	// スカラー倍
	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	static Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotate);

	static Matrix4x4 MakeTranselateMatrix(const Vector3& translate);

	// アフィン変換
	static Matrix4x4 MakeAffineMatrix(
	    const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	// 透視投影行列
	static Matrix4x4
	    MakePerspectiverFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	// 正射影行列
	static Matrix4x4 MakeOrthographicMatrix(
	    float left, float top, float right, float bottom, float nearClip, float farClip);

	// ビューポート行列
	static Matrix4x4 MakeViewportMatrix(
	    float left, float top, float width, float height, float minDepth, float maxDepth);

	// 逆行列
	static Matrix4x4 Inverse(const Matrix4x4& m);

	// 長さ(ノルマ)
	static float Length(const Vector3& v);

	// 線形補間
	static Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t);

	// 最短角補間
	static float LerpShortAngle(float a, float b, float t);

	//四角形の当たり判定
	static bool IsAABBCollision(
	    const Vector3& translate1, const Vector3 size1, const Vector3& translate2,
	    const Vector3 size2);

	
};

//inline Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }
//inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
//	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
//}
//
//inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
//	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
//}
//
//inline Vector3 operator*(const Vector3& v, float s) { return {v.x * s, v.y * s, v.z * s}; }
//inline Vector3 operator*(float s, const Vector3& v) { return {s * v.x, s * v.y, s * v.z}; }
//
//inline Vector3 operator*(const Vector3& v, const Matrix4x4& m) {
//	return {
//	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0],
//	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1],
//	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2]};
//}
//