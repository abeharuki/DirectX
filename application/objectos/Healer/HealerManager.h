#pragma once
#include "Model.h"
#include "Healer.h"

class HealerManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	const WorldTransform& GetWorldTransform();
	void SetViewProjection(const ViewProjection& viewProjection);

	//プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

private:
	WorldTransform worldTransformBase_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Healer> healer_;

	bool isDead_ = false;

	Vector3 playerPos_;
	
};
