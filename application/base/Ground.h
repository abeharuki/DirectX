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
	void Draw(ViewProjection viewprojection);

	const WorldTransform& GetWorldTransform(int i) { return worldTransform_[i]; }

	// ワールド座標を取得 地面
	Vector3 GetWorldPosition(int i);

private:
	WorldTransform worldTransform_[2];

	std::unique_ptr<Model> model_;
};