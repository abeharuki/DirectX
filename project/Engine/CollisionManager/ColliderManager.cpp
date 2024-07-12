#include "ColliderManager.h"
#include <CollisionManager/CollisionConfig.h>

void ColliderManager::Draw(const ViewProjection& camera) {
	RenderCollisionBounds(worldTransform_, camera);
	
	
}

void ColliderManager::OnCollision(Collider* collider){

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		CheckCollision(true);
	}

}

const Vector3 ColliderManager::GetWorldPosition() const {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}