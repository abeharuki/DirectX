#include "WorldTransform.h"

void WorldTransform::Initialize() {
	matWorld_ = Math::MakeIdentity4x4();
	constBuff_ =Mesh::CreateBufferResoure(Engine::GetDevice(), sizeof(ConstBufferDataWorldTransform));
	// 書き込むためのアドレスを取得
	constBuff_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
	TransferMatrix();
}

void WorldTransform::TransferMatrix() { constMap->matWorld = matWorld_; }

void WorldTransform::UpdateMatrix() {
	Matrix4x4 AffineMatrix = Math::MakeAffineMatrix(scale, rotate, translate);
	matWorld_ = AffineMatrix;
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		matWorld_ = Math::Multiply(matWorld_, parent_->matWorld_);
	}

	TransferMatrix();
}

Vector3 WorldTransform::GetWorldPos() const {
	return Vector3(matWorld_.m[3][0], matWorld_.m[3][1], matWorld_.m[3][2]);
};