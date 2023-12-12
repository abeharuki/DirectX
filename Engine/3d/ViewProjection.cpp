#include "ViewProjection.h"
#include "Mesh.h"


void ViewProjection::Initialize() {
	CreateConstBuffer();
	Map();
	UpdateMatrix();
	TransferMatrix();
}

void ViewProjection::CreateConstBuffer() {
	constBuff_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(ConstBufferDataViewProjection));
}

void ViewProjection::Map() {
	constBuff_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
}

void ViewProjection::UpdateMatrix() {
	UpdateViewMatrix();
	UpdateProjectionMatrix();
	TransferMatrix();
}

void ViewProjection::TransferMatrix() {
	constMap->view = matView;
	constMap->projection = matProjection;
	constMap->cameraPos = {matView.m[3][0], matView.m[3][1], matView.m[3][2]};
}

void ViewProjection::UpdateViewMatrix() {
	Matrix4x4 cameraMatrix = Math::MakeAffineMatrix({1.0f, 1.0f, 1.0f}, rotation_, translation_);
	matView = Math::Inverse(cameraMatrix);
}

void ViewProjection::UpdateProjectionMatrix() {
	matProjection = Math::MakePerspectiverFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}