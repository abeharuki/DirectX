#include "BaseCharacter.h"
#include <cassert>

void BaseCharacter::Initialize(const std::vector<Model*>& models) {
	models_ = models;
	worldtransform_.Initialize();
}

void BaseCharacter::Update() { worldtransform_.UpdateMatrix(); }

void BaseCharacter::Draw(const ViewProjection& viewprojection) {
	// モデル描画
	for (Model* model : models_) {
		model->Draw(worldtransform_, viewprojection);
	}
}