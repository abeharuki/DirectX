#pragma once
#include "3d/WorldTransform.h"
#include "Math/math.h"
#include <LineBox.h>

class Collider
{
public:
	virtual void OnCollision(Collider* collider) = 0;

	virtual const Vector3 GetWorldPosition() const = 0;

	virtual const WorldTransform& GetWorldTransform() const = 0;

	const float GetRadius() const { return radius_; };

	void SetRadius(float radius) { radius_ = radius; };

	const AABB& GetAABB() const { return aabb_; };

	void SetAABB(AABB& aabb) { aabb_ = aabb; SetBounds(); };

	const OBB& GetOBB() const { return obb_; };

	void SetOBB(OBB& obb) { obb_ = obb; };

	const uint32_t GetCollisionAttribute() const { return collisionAttribute_; };

	void SetCollisionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; };

	const uint32_t GetCollisionMask() const { return collisionMask_; };

	void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; };

	const uint32_t GetCollisionPrimitive() const { return collisionPrimitive_; };

	void SetCollisionPrimitive(uint32_t collisionPrimitive) { collisionPrimitive_ = collisionPrimitive; };

	void SetBounds() {
		lineBox_ = std::make_unique<LineBox>();
		lineBox_.reset(LineBox::Create(aabb_));
	};

	void RenderCollisionBounds(WorldTransform& world, const ViewProjection& camera) {
		lineBox_->Draw(world, camera,true);
	};

private:
	float radius_ = 1.0f;

	AABB aabb_{ {-1.0f,-1.0f,-1.0f},{1.0f,1.0f,1.0f} };

	OBB obb_{ {0.0f,0.0f,0.0f},{{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f}},{1.0f,1.0f,1.0f} };

	uint32_t collisionAttribute_ = 0xffffffff;

	uint32_t collisionMask_ = 0xffffffff;

	uint32_t collisionPrimitive_ = 0b1;

	std::unique_ptr<LineBox> lineBox_;
};

