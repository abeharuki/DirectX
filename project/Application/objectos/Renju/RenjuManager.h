#pragma once
#include "Model.h"
#include "Renju.h"
#include <ParticleSystem.h>


class RenjuManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);


	
	

	/*-----ゲッター-----*/
	Renju* GetRenju() { return renju_.get(); };
	

	/*-----セッター-----*/
	void SetParticlePos(Vector3 pos);
	

private:
	std::unique_ptr<Animations>animation_;
	//丸影
	WorldTransform worldTransformShadow_;
	std::unique_ptr<Model> shadowModel_;

	std::unique_ptr<Renju> renju_;
	
	//パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_;
	bool isParticle_ = false;
	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

};
