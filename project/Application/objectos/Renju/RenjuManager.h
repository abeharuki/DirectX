#pragma once
#include "Model.h"
#include "Renju.h"
#include <ParticleSystem.h>

/**
 * @file RenjuManager
 * @brief レンジャーキャラクターを管理するクラス
 */
class RenjuManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	/*-----ゲッター-----*/
	Renju* GetRenju() { return renju_.get(); };

private:
	//std::unique_ptr<Animations>animation_;

	std::unique_ptr<Renju> renju_;
};
