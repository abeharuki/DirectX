#include "RenjuParticle.h"

void RenjuParticle::Initialize(Particle* particle, Vector3& position) {
	particle_ = particle;
	particle_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	particle_->SetSpeed(5.0f);
	particle_->SetTranslate(position);
	particle_->Update();
}

void RenjuParticle::Update() { 
	
	particle_->StopParticles();
}


void RenjuParticle::Draw(const ViewProjection& viewprojection) { 
	particle_->Draw(viewprojection); 
}