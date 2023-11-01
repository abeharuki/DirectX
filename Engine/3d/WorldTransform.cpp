#include "WorldTransform.h"

void WorldTransform::Initialize() {
	matWorld_ = Math::MakeIdentity4x4();
	CreateConstBuffer();
	Map();
	TransferMatrix();
}

void WorldTransform::CreateConstBuffer() {
	constBuff_ = Mesh::CreateBufferResoure(
	    Engine::GetDevice().Get(), sizeof(ConstBufferDataWorldTransform));
}

void WorldTransform::Map() {
	constBuff_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
}

void WorldTransform::TransferMatrix() { constMap->matWorld = matWorld_; }

void WorldTransform::UpdateMatrix() {
	matWorld_ = Math::MakeAffineMatrix(scale, rotate, translate);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		matWorld_ = Math::Multiply(matWorld_, parent_->matWorld_);
	}

	TransferMatrix();
}

Vector3 WorldTransform::GetWorldPos() {
	return {matWorld_.m[3][0], matWorld_.m[3][1], matWorld_.m[3][2]};
}