#pragma once
#include "KeyInput.h"
#include "Model.h"
#include "WorldTransform.h"
#include <list>

class Skydome {
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
	void Draw(const ViewProjection& viewprojection);

private:
	WorldTransform worldTransform_;
	std::unique_ptr<Model> model_;
};