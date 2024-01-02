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
	Vector3 katanaPos();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	bool IsAttack() { return player_->IsAttack(); }

private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHammer_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Model> HammerModel_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> bulletModel_;
	bool isDead_ = false;
};