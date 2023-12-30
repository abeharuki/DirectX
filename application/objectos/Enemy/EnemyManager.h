#pragma once
#include "Enemy.h"
#include "Model.h"

class EnemyManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	const WorldTransform& GetWorldTransform();
	Vector3 GetWorldPos();

private:
	WorldTransform worldTransformBase_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Enemy> enemy_;

	bool isDead_ = false;
};