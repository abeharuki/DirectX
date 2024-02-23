#include "Ground.h"
#include <imgui.h>

void Ground::Initialize(Model* model) {
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.scale = { 1.0f, 1.0f, 1.0f };

}

/// <summary>
/// 毎フレーム処理
/// </summary>
void Ground::Update() {

	worldTransform_.UpdateMatrix();

};

/// <summary>
/// 描画
/// </summary>
void Ground::Draw(ViewProjection viewprojection, bool light) {
	model_->Draw(worldTransform_, viewprojection, light);


};

Vector3 Ground::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}