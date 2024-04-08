#pragma once
#include "Model.h"


class BaseCharacter {

public:
	virtual void Initialize(const std::vector<Model*>& models);

	virtual void Update();

	virtual void Draw(const ViewProjection& viewprojection, bool light);

	const WorldTransform& GetWorldTransform() { return worldtransform_; }

protected:
	// モデル配列データ
	std::vector<Model*> models_;
	// ワールド変換データ
	WorldTransform worldtransform_;
};