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
    /// <summary>
    /// シングルトンインスタンスを取得
    /// </summary>
    /// <returns>ParticleManagerのインスタンス</returns>
    static ParticleManager* GetInstance();

    /// <summary>
    /// シングルトンインスタンスを破棄
    /// </summary>
    static void Destroy();

    /// <summary>
    /// パーティクルシステムを新しく生成
    /// </summary>
    /// <param name="name">パーティクルシステムの名前</param>
    /// <param name="Id">パーティクルシステムの識別ID</param>
    /// <returns>生成されたParticleSystemオブジェクト</returns>
    static ParticleSystem* Create(const std::string& name, uint32_t Id);

private:
    /// <summary>
    /// ParticleManagerのコンストラクタ（デフォルト）
    /// </summary>
    ParticleManager() = default;

    /// <summary>
    /// ParticleManagerのデストラクタ
    /// </summary>
    ~ParticleManager() = default;

    /// <summary>
    /// コピーコンストラクタを削除
    /// </summary>
    ParticleManager(const ParticleManager&) = delete;

    /// <summary>
    /// コピー代入演算子を削除
    /// </summary>
    ParticleManager& operator=(const ParticleManager&) = delete;

    /// <summary>
    /// 内部でパーティクルシステムを生成
    /// </summary>
    /// <param name="name">パーティクルシステムの名前</param>
    /// <param name="Id">パーティクルシステムの識別ID</param>
    /// <returns>生成されたParticleSystemオブジェクト</returns>
    ParticleSystem* CreateInternal(const std::string& name, uint32_t Id);

private:
    /// <summary>
    /// ParticleManagerのシングルトンインスタンス
    /// </summary>
    static ParticleManager* instance_;

    /// <summary>
    /// パーティクルシステムのリスト
    /// </summary>
    std::map<ParticleId, std::unique_ptr<ParticleSystem>> particleSystems_;
};
