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
	constBuff_.Get()->Map(0, nullptr, reinterpret_cast<void **>(&constMap));
}

void ViewProjection::UpdateMatrix() {
	UpdateViewMatrix();
	UpdateProjectionMatrix();
	TransferMatrix();
}

void ViewProjection::TransferMatrix() {
	constMap->view = matView;
	constMap->projection = matProjection;
	constMap->cameraPos = { matView.m[3][0], matView.m[3][1], matView.m[3][2] };
}

void ViewProjection::UpdateViewMatrix() {
	Matrix4x4 cameraMatrix = Math::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotation_, translation_);
	matView = Math::Inverse(cameraMatrix);
	worldPos_ = { matView.m[3][0], matView.m[3][1], matView.m[3][2] };

}

void ViewProjection::UpdateProjectionMatrix() {
	matProjection = Math::MakePerspectiverFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}

std::pair<Vector3, Vector3> ViewProjection::ScreenToWorld(const Vector2 &screenPos) const
{
	const DirectXCommon *const pDxCommon = DirectXCommon::GetInstance();
	const auto &viewPort = pDxCommon->GetViewPort();

	return ScreenToWorld(screenPos, matView * matProjection * MakeViewportMatrix({}, viewPort.Width, viewPort.Height, viewPort.MinDepth, viewPort.MaxDepth));
}

Vector3 ViewProjection::WorldToScreen(const Vector3 &worldPos) const
{
	const DirectXCommon *const pDxCommon = DirectXCommon::GetInstance();
	const auto &viewPort = pDxCommon->GetViewPort();

	return WorldToScreen(worldPos, matView * matProjection * MakeViewportMatrix({}, viewPort.Width, viewPort.Height, viewPort.MinDepth, viewPort.MaxDepth));
}


std::pair<Vector3, Vector3> ViewProjection::ScreenToWorld(const Vector2 &screenPos, const Matrix4x4 &matVPVp) {
	Matrix4x4 matInvarseVPVp = InverseSRT(matVPVp);

	Vector3 posNear{ screenPos.x, screenPos.y, 0.f };
	Vector3 posFar{ screenPos.x, screenPos.y, 1.f };

	posNear = posNear * matInvarseVPVp;
	posFar = posFar * matInvarseVPVp;

	std::pair<Vector3, Vector3> segment;
	segment.first = posNear;
	segment.second = posFar;
	return segment;
}

Vector3 ViewProjection::WorldToScreen(const Vector3 &worldPos, const Matrix4x4 &matVPVp) {
	return worldPos * matVPVp;
}

Matrix4x4 ViewProjection::MakeViewportMatrix(const Vector2 &LeftTop, const float &width, const float &height, const float &minDepth, const float &maxDepth)
{
	return Matrix4x4{
			width / 2, 0, 0, 0,
			0, -height / 2, 0, 0,
			0, 0, maxDepth - minDepth, 0,
			LeftTop.x + width / 2, LeftTop.y + height / 2, minDepth, 1
	};
}
