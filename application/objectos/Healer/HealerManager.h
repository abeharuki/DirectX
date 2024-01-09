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

	Vector3 GetWorldPos() { return healer_->GetWorldTransform().GetWorldPos(); }
	Vector3 GetCanePos() { return healer_->GetWorldTransformCane().GetWorldPos(); }

	bool IsAttack() { return healer_->IsAttack(); }

	//プレイヤーに追従
	//  プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	// 敵の位置を検索
	void SetEnemypPos(Vector3 enemyPos) { enemyPos_ = enemyPos; };
	
	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);
	void OnCollision(const WorldTransform& worldTransform);

private:
	
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Model> CaneModel_;
	std::unique_ptr<Healer> healer_;
	
	bool isDead_ = false;

	Vector3 playerPos_;
	Vector3 enemyPos_;

	// 味方の判定
	bool preHit_;
	bool isHit_;
};
