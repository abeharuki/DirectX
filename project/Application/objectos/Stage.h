#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <list>
#include <CollisionManager/ColliderManager.h>
#include <Enemy/DebugEnemy.h>

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

	

	void SetLight(DirectionLight directionLight) { 
		for (int i = 0; i < 21; ++i) {
			//model_[i]->DirectionalLightDraw(directionLight);
		}

	}

	void SetDebugEnemy(DebugEnemy* debugEnemy) { debugEnemy_ = debugEnemy; }
	Vector2 GetPos(int i) { return Vector2(enemyAttackTransform_[i].translate.x, enemyAttackTransform_[i].translate.y); }
	Vector2 GetScale(int i) { return Vector2(enemyAttackTransform_[i].scale.x, enemyAttackTransform_[i].scale.y); }
	void SetUp(bool up ,int i) { upBox_[i] = true; }
	void SetDown(bool up, int i) { downBox_[i] = true; }
	static constexpr int size = 16;
private:
	
	Transform playerAttackTransform_[size];
	Transform enemyAttackTransform_[size];
	DebugEnemy* debugEnemy_;
	std::unique_ptr<Sprite> backGround_;
	std::unique_ptr<Sprite> boxSprite_[size];
	std::unique_ptr<Sprite> playerAttackSprite_[size];
	std::unique_ptr<Sprite> enemyAttackSprite_[size];
	bool returnScale_[size];
	bool hitPlayer_[size];
	bool moveScale_;
	bool upBox_[size];
	bool downBox_[size];
	int boxNum_ = 0;
};