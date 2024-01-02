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
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransform_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Model> bulletModel_;
	std::unique_ptr<Enemy> enemy_;

	bool isDead_ = false;
};