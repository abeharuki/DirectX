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

struct ParticleM {
	Transform transform;
	Vector3 velocity;
};



class Math {
public:

	// 正規化
	static Vector3 Normalize(const Vector3& v);

	static Vector3 Add(const Vector3& v1, const Vector3& v2);

	// 減算
	static Vector3 Subract(const Vector3& v1, const Vector3& v2);
	
	static Vector3 Multiply(float scalar, const Vector3& v);

	static Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

	static Matrix4x4 Subract(const Matrix4x4& m1, const Matrix4x4& m2);

	

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

/*--------------------演算子オーバーロード---------------------------*/
/*/ 二項演算子
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subract(v1, v2); }
Vector3 operator*(float s, const Vector3& v2) { return Multiply(s, v2); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return Multiply(1.0f / s, v); }
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { return Add(m1, m2); }
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { return Subract(m1, m2); }
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return Multiply(m1, m2); }
*/
// 単項演算子
//Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }
//Vector3 operator+(const Vector3& v) { return v; }
