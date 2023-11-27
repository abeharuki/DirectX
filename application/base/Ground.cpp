#include "Ground.h"

void Ground::Initialize(Model* model) {
	assert(model);
	model_.reset(model);

	for (int i = 0; i < 2; i++) {
		worldTransform_[i].Initialize();
		worldTransform_[i].translate.y = -11;
		worldTransform_[i].scale = {100, 10, 30};
	}

	worldTransform_[1].translate.z = 100;
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void Ground::Update() {

	for (int i = 0; i < 2; i++) {
		worldTransform_[i].UpdateMatrix();
	}
};

/// <summary>
/// 描画
/// </summary>
void Ground::Draw(ViewProjection viewprojection) {
	model_->Draw(worldTransform_[0], viewprojection,false);

	model_->Draw(worldTransform_[1], viewprojection,false);
};

Vector3 Ground::GetWorldPosition(int i) {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_[i].matWorld_.m[3][0];
	worldPos.y = worldTransform_[i].matWorld_.m[3][1];
	worldPos.z = worldTransform_[i].matWorld_.m[3][2];

	return worldPos;
}