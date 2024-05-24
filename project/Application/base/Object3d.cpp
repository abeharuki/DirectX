#include "Object3d.h"

Object3d* Object3d::Create(Model* model) {
	Object3d* model_ = new Object3d;
	model_->Initialize(model);
	return model_;


}


void Object3d::Initialize(Model* model) {
	assert(model);
	model_ = model;

	worldTransform_.Initialize();
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void Object3d::Update() {

	worldTransform_.UpdateMatrix();
};

/// <summary>
/// 描画
/// </summary>
void Object3d::Draw(const ViewProjection& viewprojection, bool light) {
	model_->Draw(worldTransform_, viewprojection, light);
};