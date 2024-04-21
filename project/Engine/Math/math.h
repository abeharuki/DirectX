#pragma once
#include <cassert>
#include <format>
#include <vector>
#include "Quaternion.h"

struct Vector2 final {
	float x;
	float y;
};

struct Vector3 final {
	float x;
	float y;
	float z;

	Vector3& operator*=(const float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	Vector3& operator-=(const Vector3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3& operator+=(const Vector3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3& operator/=(const float s) {
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

struct Matrix4x4 final{
	float m[4][4];	

	bool operator==(const Matrix4x4& rhs) const
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (m[i][j] != rhs.m[i][j])
				{
					return false;
				}
			}
		}
		return true;
	}

	bool operator!=(const Matrix4x4& rhs) const
	{
		return !(*this == rhs);
	}
};


struct AABB {
	Vector3 min; // 最小点
	Vector3 max; // 最大点
};

struct OBB
{
	Vector3 center;//中心点
	Vector3 orientations[3];//座標軸。正規化・直交必須
	Vector3 size;//座標軸方向の長さの半分。中心から面までの距離
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
	float shininess;
};

struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

//ノード構造体
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix{};
	std::string name;
	std::vector<Node> children;
};

struct MaterialData {

	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct CameraForGPU {
	Vector3 worldPos;
};

struct DirectionLight {
	Vector4 color;     // ライトの色
	Vector3 direction; // ライトの向き
	float intensity;   // 輝度
	bool isEnable_ = false;
	float padding[3];

};

struct PointLight {
	Vector4 color_;     // ライトの色
	Vector3 position_;//ライトの位置
	float intensity_;   // 輝度
	float radius_;//ライトの届く距離
	float decay_;//減衰
	bool isEnable_ = false;
	float padding;
};

struct SpotLight {
	Vector4 color_; //ライトの色
	Vector3 position_;//ライトの位置
	float intensity_; //輝度
	Vector3 direction_; // 方向
	float distance_;//ライトの届く距離
	float decay_;//減衰
	float cosAngle_;//
	bool isEnable_ = false;

};


struct WritingStyle {
	DirectionLight directionLight_;
	PointLight pointLight_;
	SpotLight spotLight_;
};



struct Particle_ {
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

struct Emitter {
	Transform transform;
	uint32_t count;      // 発生数　発生頻度秒に何個出すか
	float frequency;     // 発生頻度
	float frequencyTime; // 頻度用時刻　0で初期化
};

struct AccelerationField {
	Vector3 acceleration;//加速度
	AABB area;//範囲
};

class Math {
public:

	static float Dot(const Vector3& v1, const Vector3& v2);

	// 正規化
	static Vector3 Normalize(const Vector3& v);

	static Vector3 Add(const Vector3& v1, const Vector3& v2);

	// 減算
	static Vector3 Subract(const Vector3& v1, const Vector3& v2);
	
	static Vector3 Multiply(float scalar, const Vector3& v);

	static Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

	static Matrix4x4 Subract(const Matrix4x4& m1, const Matrix4x4& m2);

	static Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);

	static Matrix4x4 MakeIdentity4x4();

	// 回転X
	static Matrix4x4 MakeRotateXMatrix(float theta = 0);

	// Y
	static Matrix4x4 MakeRotateYMatrix(float theta = 0);

	// Z
	static Matrix4x4 MakeRotateZMatrix(float theta = 0);

	//クォータニオン
	static Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);
	

	static Matrix4x4 MakeScaleMatrix(Vector3 scale);

	static Matrix4x4 MakeTranslateMatrix(Vector3 translate);

	// スカラー倍
	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	// アフィン変換 Vector3
	static Matrix4x4 MakeAffineMatrix(
	    const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	//アフィン変換　クォータニオン
	static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& quaternion, const Vector3& translation);

	static Matrix4x4
	    MakeAffineRotateMatrix(const Vector3& scale, const Matrix4x4& rotate, const Vector3& translate);

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

	// 四角形の当たり判定
	static bool IsAABBCollision(
	    const Vector3& translate1, const Vector3 size1, const Vector3& translate2,
	    const Vector3 size2);

	static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
};

/*--------------------演算子オーバーロード---------------------------*/
// 二項演算子
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator+(const Vector3& v1, float s);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator*(float s, const Vector3& v2);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator/(const Vector3& v, float s);
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);

// 単項演算子
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);

