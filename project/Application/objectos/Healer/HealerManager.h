#pragma once
#include "Model.h"
#include "Healer.h"
#include <ParticleSystem.h>

/**
 * @file HealerManager
 * @brief ヒーラーキャラクターを管理するクラス
 */
class HealerManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	

	/*-----ゲッター-----*/
	Healer* GetHealer() { return healer_.get(); };
	bool IsAttack() { return healer_->IsAttack(); }


private:
	//アニメーション
	//std::unique_ptr<Animations>animation_;
	
	std::unique_ptr<Model> StaffModel_;
	std::unique_ptr<Healer> healer_;


	

	
};