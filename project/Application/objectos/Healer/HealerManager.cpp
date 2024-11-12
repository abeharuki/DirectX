#include "HealerManager.h"

void HealerManager::Initialize() {
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Healer", "Atlas.png", "healer.gltf"));

	
	healer_ = std::make_unique<Healer>();
	healer_->Initialize(animation_.get(),"Healer");

	
}

void HealerManager::Update() {
	

	//キャラクターの更新
	healer_->Update();
	
};

void HealerManager::Draw(const ViewProjection& camera) {
	healer_->Draw(camera);
	
	

	if (healer_->IsAttack()) {
		StaffModel_->Draw(healer_->GetWorldTransformCane(), camera, false);
	}
	
}



