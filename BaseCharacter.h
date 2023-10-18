#pragma once
#pragma once
#include "Model.h"
#include <vector>

class BaseCharacter {

public:
	virtual void Initialize(const std::vector<Model*>& models);

	virtual void Update();

	virtual void Draw(const ViewProjection& viewprojection);

	const WorldTransform& GetWorldTransform() { return worldtransform_; }

protected:
	// モデル配列データ
	std::vector<Model*> models_;
	// ワールド変換データ
	WorldTransform worldtransform_;
};