#include "RenjuManager.h"

void RenjuManager::Initialize() {

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Renju", "Atlas.png", "renju.gltf"));

	shadowModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));
	shadowModel_->SetBlendMode(BlendMode::kSubtract);

	
	worldTransformShadow_.Initialize();
	worldTransformShadow_.rotate = { -1.571f,0.0f,0.0f };
	worldTransformShadow_.scale = { 1.8f,1.8f,1.0f };

	renju_ = std::make_unique<Renju>();
	renju_->Initialize(animation_.get(),"Renju");

	worldTransformShadow_.translate = { renju_->GetWorldPosition().x,0.1f,renju_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();
}

void RenjuManager::Update() {
	
	//影の計算
	shadowColor_.w = 1 - (renju_->GetWorldPosition().y / 3.9f);

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { renju_->GetWorldPosition().x,0.1f,renju_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	//キャラクターの更新
	renju_->Update();
	

};

void RenjuManager::Draw(const ViewProjection& camera) {
	renju_->Draw(camera);
	shadowModel_->Draw(worldTransformShadow_, camera, false);

}

