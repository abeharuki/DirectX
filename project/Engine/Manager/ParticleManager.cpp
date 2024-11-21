#include "ParticleManager.h"

ParticleManager* ParticleManager::instance_ = nullptr;

ParticleManager* ParticleManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ParticleManager();
	}
	return instance_;
}

void ParticleManager::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

ParticleSystem* ParticleManager::Create(const std::string& name)
{
	ParticleSystem* particleSystem = ParticleManager::GetInstance()->CreateInternal(name);
	return particleSystem;
}

ParticleSystem* ParticleManager::CreateInternal(const std::string& name)
{
	// リソースが存在するかチェック
	auto it = particleSystems_.find(name);
	if (it != particleSystems_.end()) {
		
		// 既存のリソースを返す
		return particleSystems_[name].get();
	}

	
	// マップに追加
	particleSystems_[name] = std::make_unique<ParticleSystem>();
	particleSystems_[name]->Initialize(name);

	// 新規リソースを返す
	return particleSystems_[name].get();
}