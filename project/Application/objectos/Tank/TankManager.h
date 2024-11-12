#pragma once
#include "Model.h"
#include "Tank.h"
#include <ParticleSystem.h>

class TankManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	/*-----ゲッター-----*/
	Tank* GetTank() { return tank_.get(); };
	bool IsAttack() { return tank_->IsAttack(); }
	

private:
	std::unique_ptr<Animations>animation_;
	WorldTransform worldTransformShadow_;
	std::unique_ptr<Model> shadowModel_;

	std::unique_ptr<Tank> tank_;

	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

};
