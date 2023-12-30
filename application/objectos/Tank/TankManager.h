#pragma once
#include "Model.h"
#include "Tank.h"

class TankManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	const WorldTransform& GetWorldTransform();
	void SetViewProjection(const ViewProjection& viewProjection);

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);

private:
	WorldTransform worldTransformBase_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Tank> tank_;

	bool isDead_ = false;
	Vector3 playerPos_;
};
