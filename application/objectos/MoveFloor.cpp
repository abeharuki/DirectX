#include "MoveFloor.h"
#include <imgui.h>

void MoveFloor::Initialize(Model* model) {
	assert(model);
	model_.reset(model);
	worldTransform_.Initialize();
	worldTransform_.translate = {0.0f, -1.0f, 50.0f};
	// worldTransform_.scale = {7.0f, 0.2f, 7.0f};
	worldTransform_.scale = {1.0f, 1.0f, 1.0f};
	speed_ = 0.3f;
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void MoveFloor::Update() {
	Move();

	worldTransform_.UpdateMatrix();
	ImGui::Begin("Floor");
	ImGui::DragFloat4("translation", &worldTransform_.translate.x, 0.1f);
	ImGui::End();
};

/// <summary>
/// 描画
/// </summary>
void MoveFloor::Draw(const ViewProjection viewprojection) {
	model_->Draw(worldTransform_, viewprojection,true);
};

Vector3 MoveFloor::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void MoveFloor::Move() {

	if (worldTransform_.translate.z < 12.0) {
		speed_ = 0.3f;
	} else if (worldTransform_.translate.z > 88.0) {
		speed_ = -0.3f;
	}
	worldTransform_.translate.z += speed_;
}