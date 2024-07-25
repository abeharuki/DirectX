#include "RenjuParticle.h"

void RenjuParticle::Initialize(ParticleSystem* particle, Vector3& position) {
	particle_ = particle;
	particle_->SetTranslate(position);
	particle_->SetFrequencyTime(0.5f);

}

void RenjuParticle::Update() {
	particle_->Update();
}


void RenjuParticle::Draw(const ViewProjection& viewprojection) {
	particle_->Draw(viewprojection);
}