#include "HealerManager.h"

void HealerManager::Initialize() {
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Healer", "Atlas.png", "healer.gltf"));

	shadowModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));
	shadowModel_->SetBlendMode(BlendMode::kSubtract);


	worldTransformShadow_.Initialize();
	worldTransformShadow_.rotate = { -1.571f,0.0f,0.0f };
	worldTransformShadow_.scale = { 1.8f,1.8f,1.0f };

	healer_ = std::make_unique<Healer>();
	healer_->Initialize(animation_.get(),"Healer");

	worldTransformShadow_.translate = { healer_->GetWorldPosition().x,0.1f,healer_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();
}

void HealerManager::Update() {
	//影の計算
	shadowColor_.w = 1 - (healer_->GetWorldPosition().y / 3.9f);

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { healer_->GetWorldPosition().x,0.1f,healer_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	//キャラクターの更新
	healer_->Update();
	
};

void HealerManager::Draw(const ViewProjection& camera) {
	healer_->Draw(camera);
	
	shadowModel_->Draw(worldTransformShadow_, camera, false);

	if (healer_->IsAttack()) {
		StaffModel_->Draw(healer_->GetWorldTransformCane(), camera, false);
	}
	
}



