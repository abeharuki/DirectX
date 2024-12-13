#pragma once
#include "Tank.h"
#include <ParticleSystem.h>

/**
 * @file TankManager
 * @brief タンクキャラクターを管理するクラス
 */
class TankManager {
public:
	//キャラクターの生成
	void Initialize();
	//キャラクターの更新
	void Update();
	//キャラクターの描画
	void Draw(const ViewProjection& camera);

	/*-----ゲッター-----*/
	Tank* GetTank() { return tank_.get(); };
	bool IsAttack() { return tank_->IsAttack(); }
	

private:
	std::unique_ptr<Tank> tank_;
};
