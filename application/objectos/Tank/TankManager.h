#pragma once
#include "Model.h"
#include "Tank.h"

class TankManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	const WorldTransform& GetWorldTransform();
	const Vector3 GetWorldPos() { return worldTransformBase_.GetWorldPos(); }
	void SetViewProjection(const ViewProjection& viewProjection);

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	// 敵の位置を検索
	void SetEnemypPos(Vector3 enemyPos) { enemyPos_ = enemyPos; };

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	bool GetAttack() { return tank_->GetAttack(); }

private:
	WorldTransform worldTransformBase_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Tank> tank_;

	bool isDead_ = false;
	Vector3 playerPos_;
	Vector3 enemyPos_;
};
