#pragma once
#include "Collider.h"
#include <list>


class CollisionManager
{
public:
	void ClearColliderList();

	void SetColliderList(Collider* collider);

	void CheckAllCollisions();

private:
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	bool CheckCollisionSphere(const sphere& sphereA, const sphere& sphereB);

	bool CheckCollisionSphereAABB(const sphere& sphere, const AABB& aabb);

	bool CheckCollisionAABB(const AABB& aabbA, const AABB& aabbB);

	bool CheckCollisionAABBOBB(const AABB& aabb, const OBB& obb);

private:
	std::list<Collider*> colliders_{};
};