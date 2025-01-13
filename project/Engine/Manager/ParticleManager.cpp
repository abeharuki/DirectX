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
    ++particleNum;
    // 検索用のParticleIdを作成
   // ParticleId pid = {particleNum };

    // リソースが既に存在するかを検索
    auto it = particleSystems_.find(particleNum);
    if (it != particleSystems_.end()) {
        // 既存のリソースを返す
        return it->second.get();
    }

    // 新規リソースを作成
    auto particleSystem = std::make_unique<ParticleSystem>();
    particleSystem->Initialize(name);
    ParticleSystem* rawPtr = particleSystem.get(); // 生ポインタを取得

    // 新しいリソースをマップに追加
    particleSystems_[particleNum] = std::move(particleSystem);

    // 新規リソースを返す
    return rawPtr;
}

