#pragma once
#include "Model.h"
#include "Healer.h"
#include <ParticleSystem.h>

class HealerManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	

	/*-----ゲッター-----*/
	Healer* GetHealer() { return healer_.get(); };
	bool IsAttack() { return healer_->IsAttack(); }


private:
	WorldTransform worldTransformShadow_;
	//アニメーション
	std::unique_ptr<Animations>animation_;
	std::unique_ptr<Model> shadowModel_;
	std::unique_ptr<Model> StaffModel_;
	std::unique_ptr<Healer> healer_;


	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

	
};