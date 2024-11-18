#include "RenjuManager.h"

void RenjuManager::Initialize() {

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Renju", "Atlas.png", "renju.gltf"));

	bowModel_.reset(Model::CreateModelFromObj("resources/Renju/bow.obj", "resources/Renju/bow.png"));
	

	renju_ = std::make_unique<Renju>();
	renju_->Initialize(animation_.get(),"Renju");
}

void RenjuManager::Update() {
	//キャラクターの更新
	renju_->Update();
	
};

void RenjuManager::Draw(const ViewProjection& camera) {
	renju_->Draw(camera);

	if (renju_->GetState() == CharacterState::Unique || renju_->GetState() == CharacterState::Attacking) {
		bowModel_->Draw(renju_->GetWorldTransformBow(), camera, true);
	}
	
}

