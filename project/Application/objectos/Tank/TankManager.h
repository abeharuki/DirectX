#pragma once
#include "Model.h"
#include "Tank.h"
#include <ParticleSystem.h>

class TankManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	void DrawUI();


	/*-----ゲッター-----*/
	Tank* GetTank() { return tank_.get(); };
	bool IsAttack() { return tank_->IsAttack(); }


	/*-----セッター-----*/
	void SetPlayerPos(Vector3 playerPos) { playerPos_ = playerPos; }
	void SetParticlePos(Vector3 pos);
	void SetViewProjection(const ViewProjection& viewProjection) { tank_->SetViewProjection(viewProjection); }


private:
	std::unique_ptr<Animations>animation_;
	WorldTransform worldTransformShadow_;
	std::unique_ptr<Model> shadowModel_;

	std::unique_ptr<Tank> tank_;


	//パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_;
	bool isParticle_ = false;


	Vector3 playerPos_;

	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

};
