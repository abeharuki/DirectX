#include "Skydome.h"


void Skydome::Initialize(Model* model) {
	assert(model);
	model_ = model;

	worldTransform_.Initialize(); 
	uv = {
		{20.0f,10.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	
}

/// <summary>
/// 毎フレーム処理
/// </summary>
void Skydome::Update() {
	model_->SetUV(uv);
	worldTransform_.scale = {500, 500, 500};
	worldTransform_.UpdateMatrix();
	ImGui::Begin("SkyDome");
	ImGui::DragFloat3("Scale", &uv.scale.x, 0.1f);
	ImGui::End();
};

/// <summary>
/// 描画
/// </summary>
void Skydome::Draw(const ViewProjection& viewprojection, bool light) {
	model_->Draw(worldTransform_, viewprojection,light);
};