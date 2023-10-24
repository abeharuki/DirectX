#include "Skydome.h"


void Skydome::Initialize(Model* model) {
	assert(model);
	model_.reset(model);

	worldTransform_.Initialize();
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void Skydome::Update() {

	worldTransform_.scale = {500, 500, 500};
	worldTransform_.UpdateMatrix();
};

/// <summary>
/// 描画
/// </summary>
void Skydome::Draw(const ViewProjection& viewprojection, bool light) {
	model_->Draw(worldTransform_, viewprojection,light);
};