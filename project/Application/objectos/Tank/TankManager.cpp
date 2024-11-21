#include "TankManager.h"

void TankManager::Initialize() {
	
	/*animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Tank", "Atlas.png", "tank.gltf"));*/

	

	tank_ = std::make_unique<Tank>();
	tank_->Initialize(AnimationManager::Create("./resources/Tank", "Atlas.png", "tank.gltf"),"Tank");

	
}

void TankManager::Update() {
	//キャラクターの更新
	tank_->Update();
};

void TankManager::Draw(const ViewProjection& camera) {
	tank_->Draw(camera);
}





