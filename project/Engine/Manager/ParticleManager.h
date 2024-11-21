#pragma once
#include "Particle/ParticleSystem.h"

class ParticleManager {
public:
	static ParticleManager* GetInstance();

	static void Destroy();

	static ParticleSystem* Create(const std::string& name);

private:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	ParticleSystem* CreateInternal(const std::string& name);
private:
	static ParticleManager* instance_;

	std::map<std::string, std::shared_ptr<ParticleSystem>> particleSystems_;
};