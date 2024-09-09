#pragma once
#include "CollisionManager/Collider.h"



class ColliderManager :public Collider {
public:

	

	void SetManagerRadius(float radius) { 
		SetRadius(radius);
		priType = Primitive::kSphere;
	};
	void SetManagerAABB(AABB aabb) { 
		SetAABB(aabb); 
		priType = Primitive::kAABB;
	}
	void SetAttribute(uint32_t collisionAttribute) { SetCollisionAttribute(collisionAttribute); };
	void SetMask(uint32_t collisionMask) { SetCollisionMask(collisionMask); };
	void SetPrimitive(uint32_t collisionPrimitive) { SetCollisionPrimitive(collisionPrimitive); };

	void Draw(const ViewProjection& viewProjection);

	//コライダー用仮置き
	void SetWorldTransform(WorldTransform worldTransform) { worldTransform_ = worldTransform; }
	void OnCollision(Collider* collider) override;
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransform_; }
	void Chack(bool flag) { CheckCollision(flag); }
	bool GetPlayerHit() { return playerHit_; }
	void SetPlayerHit(bool hit) { playerHit_ = hit; }
	bool GetEnemyHit() { return enemyHit_; }
	void SetEnemyHit(bool hit) { enemyHit_ = hit; }
private:
	WorldTransform worldTransform_;
	Primitive priType;
	bool playerHit_;
	bool enemyHit_;
};