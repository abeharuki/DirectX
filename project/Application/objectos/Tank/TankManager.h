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
	std::unique_ptr<Tank> tank_;

};
