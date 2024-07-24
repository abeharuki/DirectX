#include "RenjuParticle.h"

void RenjuParticle::Initialize(ParticleSystem* particle, Vector3& position) {
	particle_ = particle;
	particle_->SetTranslate(position);
	particle_->Update();
}

void RenjuParticle::Update() {

	particle_->StopParticles();
}


void RenjuParticle::Draw(const ViewProjection& viewprojection) {
	particle_->Draw(viewprojection);
}