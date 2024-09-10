#pragma once
#include <cassert>
#include <format>
#include <vector>
#include "Quaternion.h"
#include <map>
#include <smmintrin.h>
#include <array>

struct Vector2 final {
	float x;
	float y;
};

static Vector2 operator+(const Vector2 v) {
	return v;
}

static Vector2 operator-(const Vector2 v) {
	return { -v.x, -v.y };
}

static Vector2 operator+(const Vector2 l, const Vector2 r) {
	return { l.x + r.x, l.y + r.y };
}

static Vector2 operator-(const Vector2 l, const Vector2 r) {
	return l + -r;
}

static Vector2 operator*(const Vector2 v, const float s) {
	return { v.x * s, v.y * s };
}

static Vector2 operator*(const float s, const Vector2 v) {
	return { v.x * s, v.y * s };
}

static Vector2 operator/(const Vector2 v, const float s) {
	return { v.x / s, v.y / s };
}

struct Vector3 final {
	float x;
	float y;
	float z;

	Vector3 &operator*=(const float s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	Vector3 &operator-=(const Vector3 &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	Vector3 &operator+=(const Vector3 &v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	Vector3 &operator/=(const float s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	Vector2 &GetVec2() { return reinterpret_cast<Vector2 &>(x); }
	const Vector2 &GetVec2() const { return reinterpret_cast<const Vector2 &>(x); }
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

	bool operator==(const Matrix4x4 &rhs) const
	{
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (m[i][j] != rhs.m[i][j]) {
					return false;
				}
			}
		}
		return true;
	}

	bool operator!=(const Matrix4x4 &rhs) const
	{
		return !(*this == rhs);
	}
};

static Matrix4x4 InverseSRT(const Matrix4x4 &mat) {

	std::array<__m128, 4> mm{};
	// データの転送
	for (uint32_t i = 0; i < 4u; i++) {
		mm[i] = _mm_load_ps(mat.m[i]);
	}

	static const constinit int32_t mask = 0b01111111;

	Vector4 alignas(16u) vecX2;
	const __m128 mX2 = __m128{ _mm_div_ps(mm[0], _mm_dp_ps(mm[0], mm[0], mask)) };
	_mm_store_ps(&vecX2.x, mX2);
	Vector4 alignas(16u) vecY2;
	const __m128 mY2 = __m128{ _mm_div_ps(mm[1], _mm_dp_ps(mm[1], mm[1], mask)) };
	_mm_store_ps(&vecY2.x, mY2);
	Vector4 alignas(16u) vecZ2;
	__m128 mZ2 = __m128{ _mm_div_ps(mm[2], _mm_dp_ps(mm[2], mm[2], mask)) };
	_mm_store_ps(&vecZ2.x, mZ2);

	return Matrix4x4{
		vecX2.x, vecY2.x, vecZ2.x, 0.f,
		vecX2.y, vecY2.y, vecZ2.y, 0.f,
		vecX2.z, vecY2.z, vecZ2.z, 0.f,
		-_mm_cvtss_f32(_mm_dp_ps(mm[3], mX2, mask)),
		 -_mm_cvtss_f32(_mm_dp_ps(mm[3], mY2, mask)),
		 -_mm_cvtss_f32(_mm_dp_ps(mm[3], mZ2, mask)), 1.f };
}

Vector3 operator*(const Vector3 &v, const Matrix4x4 &mat);


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



struct MaterialD {
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

struct MeshData
{
	std::vector<VertexData> vertices;
	std::vector<uint32_t>indices;
	uint32_t materialIndex;
};

struct MaterialData {

	std::string textureFilePath;
	Color diffuseColor;
};

struct VertexWeightData
{
	float weight;
	uint32_t vertexIndex;
};


struct JointWeightData {
	Matrix4x4 inverseBindPoseMatrix;
	std::vector<VertexWeightData> vertexWeights;


};

struct ModelData {
	std::map<std::string, JointWeightData> skinClusterData;
	MeshData meshData;
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
	float padding;
};

struct Environment {
	float environment;
	bool isEnble_ = false;
	float padding[2];
};

struct Particle {
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct ParticleCS {
	Vector3 translate;
	Vector3 scsle;
	float lifeTime;
	Vector3 velocity;
	float currentTime;
	Vector4 color;
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

struct PerView {
	Matrix4x4 view;
	Matrix4x4 projection;
	Matrix4x4 billboardMatrix;
};

struct PerFrame {
	//ゲームを起動してからの時間
	float time;
	//1フレームの経過時間
	float deltaTime;
};

class Math {
public:

	//範囲の条件文
	//valueがrangeからプラスマイナスsubの範囲か
	static bool isWithinRange(float value, float range, float sub) {
		if (value >= range - sub && value <= range + sub) {
			return true;
		}
		else {
			return false;
		}
	}

	static float Dot(const Vector3 &v1, const Vector3 &v2);

	// 正規化
	static Vector3 Normalize(const Vector3 &v);

	static Vector3 Add(const Vector3 &v1, const Vector3 &v2);

	// 減算
	static Vector3 Subract(const Vector3 &v1, const Vector3 &v2);

	static Vector3 Multiply(float scalar, const Vector3 &v);

	static Matrix4x4 Add(const Matrix4x4 &m1, const Matrix4x4 &m2);

	static Matrix4x4 Subract(const Matrix4x4 &m1, const Matrix4x4 &m2);

	static Matrix4x4 Transpose(const Matrix4x4 &m);

	static Vector3 TransformNormal(const Vector3 &vector, const Matrix4x4 &matrix);

	static Matrix4x4 MakeIdentity4x4();

	// 回転X
	static Matrix4x4 MakeRotateXMatrix(float theta = 0);

	// Y
	static Matrix4x4 MakeRotateYMatrix(float theta = 0);

	// Z
	static Matrix4x4 MakeRotateZMatrix(float theta = 0);

	//クォータニオン
	static Matrix4x4 MakeRotateMatrix(const Quaternion &quaternion);


	static Matrix4x4 MakeScaleMatrix(Vector3 scale);

	static Matrix4x4 MakeTranslateMatrix(Vector3 translate);

	// スカラー倍
	static Matrix4x4 Multiply(const Matrix4x4 &m1, const Matrix4x4 &m2);

	// アフィン変換 Vector3
	static Matrix4x4 MakeAffineMatrix(
		const Vector3 &scale, const Vector3 &rotate, const Vector3 &translate);

	//アフィン変換　クォータニオン
	static Matrix4x4 MakeAffineMatrix(const Vector3 &scale, const Quaternion &quaternion, const Vector3 &translation);

	static Matrix4x4
		MakeAffineRotateMatrix(const Vector3 &scale, const Matrix4x4 &rotate, const Vector3 &translate);

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
	static Matrix4x4 Inverse(const Matrix4x4 &m);

	// 長さ(ノルマ)
	static float Length(const Vector3 &v);

	// 線形補間
	static Vector3 Lerp(const Vector3 &p0, const Vector3 &p1, float t);

	// 最短角補間
	static float LerpShortAngle(float a, float b, float t);

	static Vector3 Cross(const Vector3 &v1, const Vector3 &v2);

	// 四角形の当たり判定
	static bool IsAABBCollision(
		const Vector3 &translate1, const Vector3 size1, const Vector3 &translate2,
		const Vector3 size2);

	// AABB を OBB に変換する関数
	static OBB ConvertAABBToOBB(AABB &aabb);

	// OBB を AABB に変換する関数
	static AABB ConvertOBBToAABB(OBB &obb);

	//AABB同士の押し出し判定
	static Vector3 PushOutAABB(const Vector3 &subjectTranslate, const AABB &subjecAABB, const Vector3 &translate, const AABB &aabb);
	//OBBに重なったAABBの押し出し判定
	static Vector3 PushOutAABBOBB(const Vector3 &aabbTranslate, const AABB &aabb, const Vector3 &obbTranslate, const OBB &obb);
	//AABBに重なったOBBの押し出し判定(出来てるかわからない)
	static Vector3 PushOutOBBAABB(const Vector3 &aabbTranslate, const AABB &aabb, const Vector3 &obbTranslate, const OBB &obb);

	static Quaternion Slerp(const Quaternion &q0, const Quaternion &q1, float t);
};

/*--------------------演算子オーバーロード---------------------------*/
// 二項演算子
Vector3 operator+(const Vector3 &v1, const Vector3 &v2);
Vector3 operator+(const Vector3 &v1, float s);
Vector3 operator-(const Vector3 &v1, const Vector3 &v2);
Vector3 operator-(const Vector3 &v1, float s);
Vector3 operator*(float s, const Vector3 &v2);
Vector3 operator*(const Vector3 &v, float s);
Vector3 operator/(const Vector3 &v, float s);
Vector3 operator*(const Vector3 &v1, const Vector3 v2);
Matrix4x4 operator+(const Matrix4x4 &m1, const Matrix4x4 &m2);
Matrix4x4 operator-(const Matrix4x4 &m1, const Matrix4x4 &m2);
Matrix4x4 operator*(const Matrix4x4 &m1, const Matrix4x4 &m2);

// 単項演算子
Vector3 operator-(const Vector3 &v);
Vector3 operator+(const Vector3 &v);

