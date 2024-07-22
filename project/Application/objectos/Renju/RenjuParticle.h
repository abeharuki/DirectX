#pragma once
#include "Particle/ParticleSystem.h"

class RenjuParticle {
public:
	void Initialize(ParticleSystem* particle, Vector3& position);

	void Update();

	void Draw(const ViewProjection& viewprojection);



private:
	ParticleSystem* particle_;
	Emitter emitter_{};
};