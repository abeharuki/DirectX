#include "RenjuManager.h"

void RenjuManager::Initialize() {

	/*animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Renju", "Atlas.png", "renju.gltf"));*/

	

	renju_ = std::make_unique<Renju>();
	renju_->Initialize(AnimationManager::Create("./resources/Renju", "Atlas.png", "renju.gltf"),"Renju");
}

void RenjuManager::Update() {
	//キャラクターの更新
	renju_->Update();
	
};

void RenjuManager::Draw(const ViewProjection& camera) {
	renju_->Draw(camera);

	
}

