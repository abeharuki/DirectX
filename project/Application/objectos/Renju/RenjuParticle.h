#pragma once
#pragma once
#include "Particle/Particle.h"

class RenjuParticle {
public:
	void Initialize(Particle* particle, Vector3& position);

	void Update();

	void Draw(const ViewProjection& viewprojection);



private:
	Particle* particle_;
	Emitter emitter_{};
};