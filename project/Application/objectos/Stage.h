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

private:
	
	Transform spriteTransform_[21];
	DebugEnemy* debugEnemy_;
	std::unique_ptr<Sprite> backGround_;
	std::unique_ptr<Sprite> boxSprite_[21];
	bool returnScale_[21];
	bool hitPlayer_[21];
	bool moveScale_;
	bool upBox_[21];
	bool downBox_[21];
};