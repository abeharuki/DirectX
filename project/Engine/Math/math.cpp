#include "math.h"

float Math::Dot(const Vector3& v1, const Vector3& v2)
{
	float result{};
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

// 加算
Vector3 Math::Add(const Vector3& v1, const Vector3& v2) {
	Vector3 add;
	add.x = v1.x + v2.x;
	add.y = v1.y + v2.y;
	add.z = v1.z + v2.z;
	return add;
};


// スカラー倍
Vector3 Math::Multiply(float scalar, const Vector3& v) {
	Vector3 multiply;
	multiply.x = v.x * scalar;
	multiply.y = v.y * scalar;
	multiply.z = v.z * scalar;
	return multiply;
};


// 正規化
Vector3 Math::Normalize(const Vector3& v) {
	Vector3 normalize;
	float mag = 1.0f / (float)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	normalize.x = v.x * mag;
	normalize.y = v.y * mag;
	normalize.z = v.z * mag;
	return normalize;
};

// 減算
Vector3 Math::Subract(const Vector3& v1, const Vector3& v2) {
	Vector3 subract;
	subract.x = v1.x - v2.x;
	subract.y = v1.y - v2.y;
	subract.z = v1.z - v2.z;
	return subract;
};

Vector3 Math::TransformNormal(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2];

	return result;
};


// 加算
Matrix4x4 Math::Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 add;
	add.m[0][0] = m1.m[0][0] + m2.m[0][0];
	add.m[0][1] = m1.m[0][1] + m2.m[0][1];
	add.m[0][2] = m1.m[0][2] + m2.m[0][2];
	add.m[0][3] = m1.m[0][3] + m2.m[0][3];
	add.m[1][0] = m1.m[1][0] + m2.m[1][0];
	add.m[1][1] = m1.m[1][1] + m2.m[1][1];
	add.m[1][2] = m1.m[1][2] + m2.m[1][2];
	add.m[1][3] = m1.m[1][3] + m2.m[1][3];
	add.m[2][0] = m1.m[2][0] + m2.m[2][0];
	add.m[2][1] = m1.m[2][1] + m2.m[2][1];
	add.m[2][2] = m1.m[2][2] + m2.m[2][2];
	add.m[2][3] = m1.m[2][3] + m2.m[2][3];
	add.m[3][0] = m1.m[3][0] + m2.m[3][0];
	add.m[3][1] = m1.m[3][1] + m2.m[3][1];
	add.m[3][2] = m1.m[3][2] + m2.m[3][2];
	add.m[3][3] = m1.m[3][3] + m2.m[3][3];
	return add;
};

// 減算
Matrix4x4 Math::Subract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 subract;
	subract.m[0][0] = m1.m[0][0] - m2.m[0][0];
	subract.m[0][1] = m1.m[0][1] - m2.m[0][1];
	subract.m[0][2] = m1.m[0][2] - m2.m[0][2];
	subract.m[0][3] = m1.m[0][3] - m2.m[0][3];
	subract.m[1][0] = m1.m[1][0] - m2.m[1][0];
	subract.m[1][1] = m1.m[1][1] - m2.m[1][1];
	subract.m[1][2] = m1.m[1][2] - m2.m[1][2];
	subract.m[1][3] = m1.m[1][3] - m2.m[1][3];
	subract.m[2][0] = m1.m[2][0] - m2.m[2][0];
	subract.m[2][1] = m1.m[2][1] - m2.m[2][1];
	subract.m[2][2] = m1.m[2][2] - m2.m[2][2];
	subract.m[2][3] = m1.m[2][3] - m2.m[2][3];
	subract.m[3][0] = m1.m[3][0] - m2.m[3][0];
	subract.m[3][1] = m1.m[3][1] - m2.m[3][1];
	subract.m[3][2] = m1.m[3][2] - m2.m[3][2];
	subract.m[3][3] = m1.m[3][3] - m2.m[3][3];
	return subract;
};


Matrix4x4 Math::MakeIdentity4x4() {
	Matrix4x4 MakeIdentity4x4;
	MakeIdentity4x4.m[0][0] = 1.0f;
	MakeIdentity4x4.m[0][1] = 0.0f;
	MakeIdentity4x4.m[0][2] = 0.0f;
	MakeIdentity4x4.m[0][3] = 0.0f;

	MakeIdentity4x4.m[1][0] = 0.0f;
	MakeIdentity4x4.m[1][1] = 1.0f;
	MakeIdentity4x4.m[1][2] = 0.0f;
	MakeIdentity4x4.m[1][3] = 0.0f;

	MakeIdentity4x4.m[2][0] = 0.0f;
	MakeIdentity4x4.m[2][1] = 0.0f;
	MakeIdentity4x4.m[2][2] = 1.0f;
	MakeIdentity4x4.m[2][3] = 0.0f;

	MakeIdentity4x4.m[3][0] = 0.0f;
	MakeIdentity4x4.m[3][1] = 0.0f;
	MakeIdentity4x4.m[3][2] = 0.0f;
	MakeIdentity4x4.m[3][3] = 1.0f;

	return MakeIdentity4x4;
};

// 回転X
Matrix4x4 Math::MakeRotateXMatrix(float theta) {
	Matrix4x4 MakeRotateMatrix;
	MakeRotateMatrix.m[0][0] = 1;
	MakeRotateMatrix.m[0][1] = 0;
	MakeRotateMatrix.m[0][2] = 0;
	MakeRotateMatrix.m[0][3] = 0;
	MakeRotateMatrix.m[1][0] = 0;
	MakeRotateMatrix.m[1][1] = std::cos(theta);
	MakeRotateMatrix.m[1][2] = std::sin(theta);
	MakeRotateMatrix.m[1][3] = 0;
	MakeRotateMatrix.m[2][0] = 0;
	MakeRotateMatrix.m[2][1] = -std::sin(theta);
	MakeRotateMatrix.m[2][2] = std::cos(theta);
	MakeRotateMatrix.m[2][3] = 0;
	MakeRotateMatrix.m[3][0] = 0;
	MakeRotateMatrix.m[3][1] = 0;
	MakeRotateMatrix.m[3][2] = 0;
	MakeRotateMatrix.m[3][3] = 1;
	return MakeRotateMatrix;
}

// Y
Matrix4x4 Math::MakeRotateYMatrix(float theta) {
	Matrix4x4 MakeRotateMatrix;
	MakeRotateMatrix.m[0][0] = std::cos(theta);
	MakeRotateMatrix.m[0][1] = 0;
	MakeRotateMatrix.m[0][2] = -std::sin(theta);
	MakeRotateMatrix.m[0][3] = 0;
	MakeRotateMatrix.m[1][0] = 0;
	MakeRotateMatrix.m[1][1] = 1;
	MakeRotateMatrix.m[1][2] = 0;
	MakeRotateMatrix.m[1][3] = 0;
	MakeRotateMatrix.m[2][0] = std::sin(theta);
	MakeRotateMatrix.m[2][1] = 0;
	MakeRotateMatrix.m[2][2] = std::cos(theta);
	MakeRotateMatrix.m[2][3] = 0;
	MakeRotateMatrix.m[3][0] = 0;
	MakeRotateMatrix.m[3][1] = 0;
	MakeRotateMatrix.m[3][2] = 0;
	MakeRotateMatrix.m[3][3] = 1;
	return MakeRotateMatrix;
}

// Z
Matrix4x4 Math::MakeRotateZMatrix(float theta) {
	Matrix4x4 MakeRotateMatrix;
	MakeRotateMatrix.m[0][0] = std::cos(theta);
	MakeRotateMatrix.m[0][1] = std::sin(theta);
	MakeRotateMatrix.m[0][2] = 0;
	MakeRotateMatrix.m[0][3] = 0;
	MakeRotateMatrix.m[1][0] = -std::sin(theta);
	MakeRotateMatrix.m[1][1] = std::cos(theta);
	MakeRotateMatrix.m[1][2] = 0;
	MakeRotateMatrix.m[1][3] = 0;
	MakeRotateMatrix.m[2][0] = 0;
	MakeRotateMatrix.m[2][1] = 0;
	MakeRotateMatrix.m[2][2] = 1;
	MakeRotateMatrix.m[2][3] = 0;
	MakeRotateMatrix.m[3][0] = 0;
	MakeRotateMatrix.m[3][1] = 0;
	MakeRotateMatrix.m[3][2] = 0;
	MakeRotateMatrix.m[3][3] = 1;
	return MakeRotateMatrix;
}

Matrix4x4 Math::MakeRotateMatrix(const Quaternion& quaternion)
{
	Matrix4x4 result{};
	result.m[0][0] = quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z;
	result.m[0][1] = 2.0f * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
	result.m[0][2] = 2.0f * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
	result.m[0][3] = 0.0f;
	result.m[1][0] = 2.0f * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
	result.m[1][1] = quaternion.w * quaternion.w - quaternion.x * quaternion.x + quaternion.y * quaternion.y - quaternion.z * quaternion.z;
	result.m[1][2] = 2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
	result.m[1][3] = 0.0f;
	result.m[2][0] = 2.0f * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
	result.m[2][1] = 2.0f * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
	result.m[2][2] = quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z;
	result.m[2][3] = 0.0f;
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;
	return result;
}


Matrix4x4 Math::MakeScaleMatrix(Vector3 scale) {
	Matrix4x4 MakeAffineMatrix;
	MakeAffineMatrix.m[0][0] = scale.x;
	MakeAffineMatrix.m[0][1] = 0;
	MakeAffineMatrix.m[0][2] = 0;
	MakeAffineMatrix.m[0][3] = 0;
	MakeAffineMatrix.m[1][0] = 0;
	MakeAffineMatrix.m[1][1] = scale.y;
	MakeAffineMatrix.m[1][2] = 0;
	MakeAffineMatrix.m[1][3] = 0;
	MakeAffineMatrix.m[2][0] = 0;
	MakeAffineMatrix.m[2][1] = 0;
	MakeAffineMatrix.m[2][2] = scale.z;
	MakeAffineMatrix.m[2][3] = 0;
	MakeAffineMatrix.m[3][0] = 0;
	MakeAffineMatrix.m[3][1] = 0;
	MakeAffineMatrix.m[3][2] = 0;
	MakeAffineMatrix.m[3][3] = 1;

	return MakeAffineMatrix;
}

Matrix4x4 Math::MakeTranslateMatrix(Vector3 translate) {
	Matrix4x4 MakeAffineMatrix;
	MakeAffineMatrix.m[0][0] = 1.0f;
	MakeAffineMatrix.m[0][1] = 0;
	MakeAffineMatrix.m[0][2] = 0;
	MakeAffineMatrix.m[0][3] = 0;
	MakeAffineMatrix.m[1][0] = 0;
	MakeAffineMatrix.m[1][1] = 1.0f;
	MakeAffineMatrix.m[1][2] = 0;
	MakeAffineMatrix.m[1][3] = 0;
	MakeAffineMatrix.m[2][0] = 0;
	MakeAffineMatrix.m[2][1] = 0;
	MakeAffineMatrix.m[2][2] = 1.0f;
	MakeAffineMatrix.m[2][3] = 0;
	MakeAffineMatrix.m[3][0] = translate.x;
	MakeAffineMatrix.m[3][1] = translate.y;
	MakeAffineMatrix.m[3][2] = translate.z;
	MakeAffineMatrix.m[3][3] = 1;

	return MakeAffineMatrix;
}

// スカラー倍
Matrix4x4 Math::Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 multiply;
	multiply.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] +
	                   m1.m[0][3] * m2.m[3][0];
	multiply.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] +
	                   m1.m[0][3] * m2.m[3][1];
	multiply.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] +
	                   m1.m[0][3] * m2.m[3][2];
	multiply.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] +
	                   m1.m[0][3] * m2.m[3][3];

	multiply.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] +
	                   m1.m[1][3] * m2.m[3][0];
	multiply.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] +
	                   m1.m[1][3] * m2.m[3][1];
	multiply.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] +
	                   m1.m[1][3] * m2.m[3][2];
	multiply.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] +
	                   m1.m[1][3] * m2.m[3][3];

	multiply.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] +
	                   m1.m[2][3] * m2.m[3][0];
	multiply.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] +
	                   m1.m[2][3] * m2.m[3][1];
	multiply.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] +
	                   m1.m[2][3] * m2.m[3][2];
	multiply.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] +
	                   m1.m[2][3] * m2.m[3][3];

	multiply.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] +
	                   m1.m[3][3] * m2.m[3][0];
	multiply.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] +
	                   m1.m[3][3] * m2.m[3][1];
	multiply.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] +
	                   m1.m[3][3] * m2.m[3][2];
	multiply.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] +
	                   m1.m[3][3] * m2.m[3][3];

	return multiply;
};

// アフィン変換
Matrix4x4
    Math::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	// 回転
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	Matrix4x4 MakeAffineMatrix;
	MakeAffineMatrix.m[0][0] = scale.x * rotateXYZMatrix.m[0][0];
	MakeAffineMatrix.m[0][1] = scale.x * rotateXYZMatrix.m[0][1];
	MakeAffineMatrix.m[0][2] = scale.x * rotateXYZMatrix.m[0][2];
	MakeAffineMatrix.m[0][3] = 0;
	MakeAffineMatrix.m[1][0] = scale.y * rotateXYZMatrix.m[1][0];
	MakeAffineMatrix.m[1][1] = scale.y * rotateXYZMatrix.m[1][1];
	MakeAffineMatrix.m[1][2] = scale.y * rotateXYZMatrix.m[1][2];
	MakeAffineMatrix.m[1][3] = 0;
	MakeAffineMatrix.m[2][0] = scale.z * rotateXYZMatrix.m[2][0];
	MakeAffineMatrix.m[2][1] = scale.z * rotateXYZMatrix.m[2][1];
	MakeAffineMatrix.m[2][2] = scale.z * rotateXYZMatrix.m[2][2];
	MakeAffineMatrix.m[2][3] = 0;
	MakeAffineMatrix.m[3][0] = translate.x;
	MakeAffineMatrix.m[3][1] = translate.y;
	MakeAffineMatrix.m[3][2] = translate.z;
	MakeAffineMatrix.m[3][3] = 1;
	return MakeAffineMatrix;
}

// アフィン変換
Matrix4x4
    Math::MakeAffineRotateMatrix(const Vector3& scale, const Matrix4x4& rotate, const Vector3& translate) {
	// 回転
	
	Matrix4x4 rotateXYZMatrix = rotate;

	Matrix4x4 MakeAffineMatrix;
	MakeAffineMatrix.m[0][0] = scale.x * rotateXYZMatrix.m[0][0];
	MakeAffineMatrix.m[0][1] = scale.x * rotateXYZMatrix.m[0][1];
	MakeAffineMatrix.m[0][2] = scale.x * rotateXYZMatrix.m[0][2];
	MakeAffineMatrix.m[0][3] = 0;
	MakeAffineMatrix.m[1][0] = scale.y * rotateXYZMatrix.m[1][0];
	MakeAffineMatrix.m[1][1] = scale.y * rotateXYZMatrix.m[1][1];
	MakeAffineMatrix.m[1][2] = scale.y * rotateXYZMatrix.m[1][2];
	MakeAffineMatrix.m[1][3] = 0;
	MakeAffineMatrix.m[2][0] = scale.z * rotateXYZMatrix.m[2][0];
	MakeAffineMatrix.m[2][1] = scale.z * rotateXYZMatrix.m[2][1];
	MakeAffineMatrix.m[2][2] = scale.z * rotateXYZMatrix.m[2][2];
	MakeAffineMatrix.m[2][3] = 0;
	MakeAffineMatrix.m[3][0] = translate.x;
	MakeAffineMatrix.m[3][1] = translate.y;
	MakeAffineMatrix.m[3][2] = translate.z;
	MakeAffineMatrix.m[3][3] = 1;
	return MakeAffineMatrix;
}

Matrix4x4 Math::MakeAffineMatrix(const Vector3& scale, const Quaternion& quaternion, const Vector3& translation)
{
	Matrix4x4 result{};
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateMatrix = MakeRotateMatrix(quaternion);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation);
	result = scaleMatrix * rotateMatrix +translateMatrix;
	return result;
}


// 透視投影行列
Matrix4x4
    Math::MakePerspectiverFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 MakePerspectiverFovMatrix;
	MakePerspectiverFovMatrix.m[0][0] = 1 / aspectRatio * (1 / std::tan(fovY / 2));
	MakePerspectiverFovMatrix.m[0][1] = 0.0f;
	MakePerspectiverFovMatrix.m[0][2] = 0.0f;
	MakePerspectiverFovMatrix.m[0][3] = 0.0f;

	MakePerspectiverFovMatrix.m[1][0] = 0.0f;
	MakePerspectiverFovMatrix.m[1][1] = 1 / std::tan(fovY / 2);
	MakePerspectiverFovMatrix.m[1][2] = 0.0f;
	MakePerspectiverFovMatrix.m[1][3] = 0.0f;

	MakePerspectiverFovMatrix.m[2][0] = 0.0f;
	MakePerspectiverFovMatrix.m[2][1] = 0.0f;
	MakePerspectiverFovMatrix.m[2][2] = farClip / (farClip - nearClip);
	MakePerspectiverFovMatrix.m[2][3] = 1.0f;

	MakePerspectiverFovMatrix.m[3][0] = 0.0f;
	MakePerspectiverFovMatrix.m[3][1] = 0.0f;
	MakePerspectiverFovMatrix.m[3][2] = -nearClip * farClip / (farClip - nearClip);
	MakePerspectiverFovMatrix.m[3][3] = 0.0f;

	return MakePerspectiverFovMatrix;
}

// 正射影行列
Matrix4x4 Math::MakeOrthographicMatrix(
    float left, float top, float right, float bottom, float nearClip, float farClip) {

	assert(left != right);
	assert(top != bottom);
	Matrix4x4 MakeOrthographicMatrix;
	MakeOrthographicMatrix.m[0][0] = 2.0f / (right - left);
	MakeOrthographicMatrix.m[0][1] = 0.0f;
	MakeOrthographicMatrix.m[0][2] = 0.0f;
	MakeOrthographicMatrix.m[0][3] = 0.0f;

	MakeOrthographicMatrix.m[1][0] = 0.0f;
	MakeOrthographicMatrix.m[1][1] = 2.0f / (top - bottom);
	MakeOrthographicMatrix.m[1][2] = 0.0f;
	MakeOrthographicMatrix.m[1][3] = 0.0f;

	MakeOrthographicMatrix.m[2][0] = 0.0f;
	MakeOrthographicMatrix.m[2][1] = 0.0f;
	MakeOrthographicMatrix.m[2][2] = 1 / (farClip - nearClip);
	MakeOrthographicMatrix.m[2][3] = 0.0f;

	MakeOrthographicMatrix.m[3][0] = (left + right) / (left - right);
	MakeOrthographicMatrix.m[3][1] = (top + bottom) / (bottom - top);
	MakeOrthographicMatrix.m[3][2] = nearClip / (nearClip - farClip);
	MakeOrthographicMatrix.m[3][3] = 1.0f;

	return MakeOrthographicMatrix;
}

// ビューポート行列
Matrix4x4 Math::MakeViewportMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 MakeViewportMatrix;
	MakeViewportMatrix.m[0][0] = width / 2.0f;
	MakeViewportMatrix.m[0][1] = 0.0f;
	MakeViewportMatrix.m[0][2] = 0.0f;
	MakeViewportMatrix.m[0][3] = 0.0f;

	MakeViewportMatrix.m[1][0] = 0.0f;
	MakeViewportMatrix.m[1][1] = -height / 2.0f;
	MakeViewportMatrix.m[1][2] = 0.0f;
	MakeViewportMatrix.m[1][3] = 0.0f;

	MakeViewportMatrix.m[2][0] = 0.0f;
	MakeViewportMatrix.m[2][1] = 0.0f;
	MakeViewportMatrix.m[2][2] = maxDepth - minDepth;
	MakeViewportMatrix.m[2][3] = 0.0f;
	MakeViewportMatrix.m[3][0] = left + (width / 2.0f);

	MakeViewportMatrix.m[3][1] = top + (height / 2.0f);
	MakeViewportMatrix.m[3][2] = minDepth;
	MakeViewportMatrix.m[3][3] = 1.0f;
	return MakeViewportMatrix;
}

// 逆行列
Matrix4x4 Math::Inverse(const Matrix4x4& m) {
	Matrix4x4 result;
	float determinant = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
	                    m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
	                    m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -
	                    m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
	                    m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
	                    m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -
	                    m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
	                    m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
	                    m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +
	                    m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
	                    m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
	                    m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
	                    m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
	                    m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
	                    m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -
	                    m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
	                    m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
	                    m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -
	                    m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
	                    m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
	                    m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +
	                    m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
	                    m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
	                    m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];
	double determinantRecp = 0;
	determinantRecp = 1.0f / determinant;

	result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
	                  m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
	                  m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]) /
	                 determinant;

	result.m[0][1] = (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
	                  m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
	                  m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]) /
	                 determinant;

	result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
	                  m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
	                  m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]) /
	                 determinant;

	result.m[0][3] = (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
	                  m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
	                  m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]) /
	                 determinant;

	result.m[1][0] = (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
	                  m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
	                  m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]) /
	                 determinant;

	result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
	                  m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
	                  m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]) /
	                 determinant;

	result.m[1][2] = (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
	                  m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
	                  m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]) /
	                 determinant;

	result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
	                  m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
	                  m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]) /
	                 determinant;

	result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
	                  m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
	                  m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]) /
	                 determinant;

	result.m[2][1] = (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
	                  m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] +
	                  m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]) /
	                 determinant;

	result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
	                  m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
	                  m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]) /
	                 determinant;

	result.m[2][3] = (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
	                  m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
	                  m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]) /
	                 determinant;

	result.m[3][0] = (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
	                  m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
	                  m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]) /
	                 determinant;

	result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
	                  m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
	                  m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]) /
	                 determinant;

	result.m[3][2] = (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
	                  m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
	                  m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]) /
	                 determinant;

	result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
	                  m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
	                  m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]) /
	                 determinant;
	return result;
};


// 長さ(ノルマ)
float Math::Length(const Vector3& v) {
	float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return length;
};



// 線形補間
Vector3 Math::Lerp(const Vector3& p0, const Vector3& p1, float t) {
	return {
	    (1.0f - t) * p0.x + t * p1.x,
	    (1.0f - t) * p0.y + t * p1.y,
	    (1.0f - t) * p0.z + t * p1.z,
	};
}

// 最短角度補間
float Math::LerpShortAngle(float a, float b, float t) {
	float Lerp = 0.0f;
	float Pi = 3.1415f;
	// 角度差分を求める
	float diff = b - a;

	if (Pi < diff) {
		Lerp = std::fmod(diff - 2.0f * Pi, 2.0f * Pi);
	} else if (-Pi > diff) {
		Lerp = std::fmod(diff + 2.0f * Pi, 2.0f * Pi);
	} else {
		Lerp = diff;
	}

	return a + Lerp * t;
}

// 四角形の当たり判定
bool Math::IsAABBCollision(
    const Vector3& translate1, const Vector3 size1, const Vector3& translate2,
    const Vector3 size2) {
	bool collision = false;
	AABB a, b;
	a = {
	    {translate1.x - size1.x, translate1.y - size1.y, translate1.z - size1.z},
	    {translate1.x + size1.x, translate1.y + size1.y, translate1.z + size1.z}
    };

	b = {
	    {translate2.x - size2.x, translate2.y - size2.y, translate2.z - size2.z},
	    {translate2.x + size2.x, translate2.y + size2.y, translate2.z + size2.z}
    };
	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) && (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
	    (a.min.z <= b.max.z && a.max.z >= b.min.z)) {

		collision = true;
	}

	return collision;
}

Quaternion Math::Slerp(const Quaternion& q0, const Quaternion& q1, float t)
{

	Quaternion result{};
	Quaternion localQ0 = q0;
	Quaternion localQ1 = q1;
	float dot = localQ0.x * localQ1.x + localQ0.y * localQ1.y + localQ0.z * localQ1.z + localQ0.w * localQ1.w;
	if (dot < 0.0f)
	{
		localQ0 = { -localQ0.x,-localQ0.y,-localQ0.z,-localQ0.w };
		dot = -dot;
	}
	if (dot >= 1.0f - std::numeric_limits<float>::epsilon())
	{
		result.x = (1.0f - t) * localQ0.x + t * localQ1.x;
		result.y = (1.0f - t) * localQ0.y + t * localQ1.y;
		result.z = (1.0f - t) * localQ0.z + t * localQ1.z;
		result.w = (1.0f - t) * localQ0.w + t * localQ1.w;
		return result;
	}
	float theta = std::acos(dot);
	float scale0 = std::sin((1 - t) * theta) / std::sin(theta);
	float scale1 = std::sin(t * theta) / std::sin(theta);
	result.x = scale0 * localQ0.x + scale1 * localQ1.x;
	result.y = scale0 * localQ0.y + scale1 * localQ1.y;
	result.z = scale0 * localQ0.z + scale1 * localQ1.z;
	result.w = scale0 * localQ0.w + scale1 * localQ1.w;
	return result;

}


/*--------------------演算子オーバーロード---------------------------*/
// 二項演算子
Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Math::Add(v1, v2); }
Vector3 operator+(const Vector3& v1, float s) { return {v1.x + s, v1.y + s, v1.z + s}; }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Math::Subract(v1, v2); }
Vector3 operator*(float s, const Vector3& v2) { return Math::Multiply(s, v2); }
Vector3 operator*(const Vector3& v, float s) { return s * v; }
Vector3 operator/(const Vector3& v, float s) { return Math::Multiply(1.0f / s, v); }
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) { return Math::Add(m1, m2); }
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) { return Math::Subract(m1, m2); }
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) { return Math::Multiply(m1, m2); }

// 単項演算子
 Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }
 Vector3 operator+(const Vector3& v) { return v; }