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
	//キャラクターの生成
	void Initialize();
	//キャラクターの更新
	void Update();
	//キャラクターの描画
	void Draw(const ViewProjection& camera);

	/*-----ゲッター-----*/
	Renju* GetRenju() { return renju_.get(); };

private:
	//std::unique_ptr<Animations>animation_;

	std::unique_ptr<Renju> renju_;
};
