#pragma once
#include "Particle/ParticleSystem.h"

/**
 * @struct ParticleId
 * @brief パーティクルシステムを一意に識別するための識別子構造体
 */
struct ParticleId {
	std::string name;
	uint32_t Id;

	// 比較演算子をオーバーロードして、std::mapで使えるようにする
	bool operator<(const ParticleId& other) const {
		return name < other.name || (name == other.name && Id < other.Id);
	}
};

/**
 * @class ParticleManager
 * @brief パーティクルシステムの管理を行うクラス
 */
class ParticleManager {
public:
	static ParticleManager* GetInstance();

	static void Destroy();

	static ParticleSystem* Create(const std::string& name, uint32_t Id);



private:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	ParticleSystem* CreateInternal(const std::string& name,uint32_t Id);
private:
	static ParticleManager* instance_;

	std::map<ParticleId, std::unique_ptr<ParticleSystem>> particleSystems_;
};