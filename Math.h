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

struct sphere {
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

	// 減算
	static Vector3 Subract(const Vector3& v1, const Vector3& v2);
	
	static Matrix4x4 MakeIdentity4x4();

	// 回転X
	static Matrix4x4 MakeRotateXMatrix(float theta = 0);

	// Y
	static Matrix4x4 MakeRotateYMatrix(float theta = 0);

	// Z
	static Matrix4x4 MakeRotateZMatrix(float theta = 0);

	// スカラー倍
	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

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


};