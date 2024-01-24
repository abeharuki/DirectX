#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <list>

class Ground {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection viewprojection,bool light);

	const WorldTransform& GetWorldTransform() {}

	// ワールド座標を取得 地面
	Vector3 GetWorldPosition();

private:
	WorldTransform worldTransform_;

    Model* model_;
};