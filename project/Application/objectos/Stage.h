#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <list>
#include <CollisionManager/ColliderManager.h>
#include <Enemy/DebugEnemy.h>
#include <bitset>

class Stage{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection viewprojection, bool light);

	

	void SetShake(Vector2 shake) { 
		shake_ = shake;
	}

	void SetDebugEnemy(DebugEnemy* debugEnemy) { debugEnemy_ = debugEnemy; }
	Vector2 GetPos(int i) { return Vector2(enemyAttackTransform_[i].translate.x, enemyAttackTransform_[i].translate.y); }
	Vector2 GetScale(int i) { return Vector2(enemyAttackTransform_[i].scale.x, enemyAttackTransform_[i].scale.y); }
	void SetUp(bool up ,int i) {
		if (!downBox_[i]) {
			upBox_[i] = true;
		}
		
	}
	void SetDown(bool up, int i) {
		if (!upBox_[i]) {
			downBox_[i] = true;
		}
		
	}
	bool GetUp(uint32_t index) const { return upBox_.at(index); }
	bool GetDown(uint32_t index) const { return downBox_.at(index); }
	static constexpr int32_t kSize_ = 16;
private:
	
	Vector2 boxPos_[kSize_];
	Transform playerAttackTransform_[kSize_];
	Transform enemyAttackTransform_[kSize_];
	DebugEnemy* debugEnemy_;
	std::unique_ptr<Sprite> backGround_;
	std::unique_ptr<Sprite> boxSprite_[kSize_];
	std::unique_ptr<Sprite> playerAttackSprite_[kSize_];
	std::unique_ptr<Sprite> enemyAttackSprite_[kSize_];
	std::array<bool ,kSize_> returnScale_;
	std::array<bool, kSize_> hitPlayer_;
	bool moveScale_;
	std::array<bool ,kSize_> upBox_;
	std::array<bool ,kSize_> downBox_;
	int boxNum_ = 0;
	Vector2 shake_;
	float posY[kSize_];
};