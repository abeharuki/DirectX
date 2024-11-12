#include "TankManager.h"

void TankManager::Initialize() {
	
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Tank", "Atlas.png", "tank.gltf"));

	shadowModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));
	shadowModel_->SetBlendMode(BlendMode::kSubtract);

	worldTransformShadow_.Initialize();
	worldTransformShadow_.rotate = { -1.571f,0.0f,0.0f };
	worldTransformShadow_.scale = { 2.2f,2.2f,1.0f };

	tank_ = std::make_unique<Tank>();
	tank_->Initialize(animation_.get(),"Tank");

	worldTransformShadow_.translate = { tank_->GetWorldPosition().x,0.1f,tank_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();
}

void TankManager::Update() {
	
	//影の計算
	shadowColor_.w = 1 - (tank_->GetWorldPosition().y / 3.9f);

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { tank_->GetWorldPosition().x,0.1f,tank_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	//キャラクターの更新
	tank_->Update();
	
	
	
};

void TankManager::Draw(const ViewProjection& camera) {
	tank_->Draw(camera);
	shadowModel_->Draw(worldTransformShadow_, camera, false);
}





