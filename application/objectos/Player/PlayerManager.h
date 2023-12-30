#pragma once
#include "Player.h"
#include "Model.h"

class PlayerManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	const WorldTransform& GetWorldTransform();
	void SetViewProjection(const ViewProjection* viewProjection);

	Vector3 GetWorldPos();

private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHammer_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Model> HammerModel_;
	std::unique_ptr<Player> player_;

	bool isDead_ = false;
};