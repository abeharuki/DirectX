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
	Vector3 GetCanePos() { return healer_->GetWorldTransformCane().GetWorldPos(); }

	bool IsAttack() { return healer_->IsAttack(); }

	/*-----セッター-----*/
	//パーティクルのpos
	void SetParticlePos(Vector3 pos);

private:
	WorldTransform worldTransformShadow_;
	//アニメーション
	std::unique_ptr<Animations>animation_;
	std::unique_ptr<Model> shadowModel_;
	std::unique_ptr<Model> StaffModel_;
	std::unique_ptr<Healer> healer_;

	//パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_;
	bool isParticle_ = false;

	
	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

	
};