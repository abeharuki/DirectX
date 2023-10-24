#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <list>

class MoveFloor {
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

	void Move();

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	// ワールド座標を取得 地面
	Vector3 GetWorldPosition();

private:
	WorldTransform worldTransform_;
	std::unique_ptr<Model> model_;

	float speed_ = 0.0f;
};