#pragma once
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include <list>

class Object3d {
public:
	static Object3d* Create(Model* model);


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
	void Draw(const ViewProjection& viewprojection, bool light);

	void SetPosition(Vector3 pos) { worldTransform_.translate = pos; }
	void SetRatation(Vector3 rotato) { worldTransform_.rotate= rotato; }
	void SetScale(Vector3 size) { worldTransform_.scale = size; }

private:
	WorldTransform worldTransform_;
	Model* model_;
};
