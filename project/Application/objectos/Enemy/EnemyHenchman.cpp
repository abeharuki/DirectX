#include "EnemyHenchman.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

void EnemyHenchman::Init(Animations* animation,Vector3 pos)
{
	worldTransform_.Initialize();
	worldTransform_.translate = pos;
	animation_ = animation;
	

	AABB aabbSize{ .min{-1.0f,-1.0f,-1.0f},.max{1.0f,2.f,1.0f} };
	OBB obb = Math::ConvertAABBToOBB(aabbSize);
	obb.center = { 0.0f,0.8f,0.0f };
	SetOBB(obb);
	SetCollisionPrimitive(kCollisionPrimitiveOBB);
	SetCollisionAttribute(kCollisionAttributeHenchman);
	SetCollisionMask(kCollisionMaskHenchman);

}

void EnemyHenchman::Update(){
	--time_;
	if (time_ <= 0) {
		dead_ = true;
	}
	
	worldTransform_.rotate.y = Math::LerpShortAngle(worldTransform_.rotate.y, destinationAngleY_, 0.2f);
	if (!hit_) {
		
	}
	followRenju();
	if (worldTransform_.translate.x == renjuPos_.x || worldTransform_.translate.z == renjuPos_.z) {
		dead_ = true;
	}
	worldTransform_.UpdateMatrix();

}

void EnemyHenchman::Draw(const ViewProjection& camera)
{
	animation_->Draw(worldTransform_, camera, false);
	RenderCollisionBounds(worldTransform_, camera);
}

void EnemyHenchman::followRenju(){

	if (worldTransform_.translate.y < 0) {
		worldTransform_.translate.y += 0.1f;
	}
	else {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = renjuPos_ - GetWorldPosition();

		// y軸周りの回転
		if (sub.z != 0.0) {
			destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

			if (sub.z < 0.0) {
				destinationAngleY_ = (sub.x >= 0.0)
					? std::numbers::pi_v<float> -destinationAngleY_
					: -std::numbers::pi_v<float> -destinationAngleY_;
			}
		}
		else {
			destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f
				: -std::numbers::pi_v<float> / 2.0f;
		}

		worldTransform_.translate = Math::Lerp(worldTransform_.translate, renjuPos_, 0.005f);
		worldTransform_.translate.y = 0.0f;
	}
}

const Vector3 EnemyHenchman::GetWorldPosition() const{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void EnemyHenchman::OnCollision(Collider* collider) {
	hit_ = false;
	//プレイヤーとの押し出し処理
	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		hit_ = true;
		OBB obb = {
			.center{collider->GetOBB().center.x + collider->GetWorldPosition().x,collider->GetOBB().center.y + collider->GetWorldPosition().y,collider->GetOBB().center.z + collider->GetWorldPosition().z},
			.orientations{
				 {Vector3{collider->GetWorldTransform().matWorld_.m[0][0],collider->GetWorldTransform().matWorld_.m[0][1],collider->GetWorldTransform().matWorld_.m[0][2]}},
				 {Vector3{collider->GetWorldTransform().matWorld_.m[1][0],collider->GetWorldTransform().matWorld_.m[1][1],collider->GetWorldTransform().matWorld_.m[1][2]}},
				 {Vector3{collider->GetWorldTransform().matWorld_.m[2][0],collider->GetWorldTransform().matWorld_.m[2][1],collider->GetWorldTransform().matWorld_.m[2][2]}},
			},
			.size{collider->GetOBB().size}
		};
		worldTransform_.translate += Math::PushOutAABBOBB(worldTransform_.translate, GetAABB(), collider->GetWorldTransform().translate, obb) * 0.3f;

	}
	if (collider->GetCollisionAttribute() == kCollisionAttributeTank) {
		hit_ = true;
		OBB obb = {
			.center{collider->GetOBB().center.x + collider->GetWorldPosition().x,collider->GetOBB().center.y + collider->GetWorldPosition().y,collider->GetOBB().center.z + collider->GetWorldPosition().z},
			.orientations{
				 {Vector3{collider->GetWorldTransform().matWorld_.m[0][0],collider->GetWorldTransform().matWorld_.m[0][1],collider->GetWorldTransform().matWorld_.m[0][2]}},
				 {Vector3{collider->GetWorldTransform().matWorld_.m[1][0],collider->GetWorldTransform().matWorld_.m[1][1],collider->GetWorldTransform().matWorld_.m[1][2]}},
				 {Vector3{collider->GetWorldTransform().matWorld_.m[2][0],collider->GetWorldTransform().matWorld_.m[2][1],collider->GetWorldTransform().matWorld_.m[2][2]}},
			},
			.size{collider->GetOBB().size}
		};
		worldTransform_.translate += Math::PushOutAABBOBB(worldTransform_.translate, GetAABB(), collider->GetWorldTransform().translate, obb)*0.3f;

	}
	

	if (collider->GetCollisionAttribute() == kCollisionAttributeLoderWall) {

		OBB obb = {
			.center{collider->GetOBB().center.x + collider->GetWorldPosition().x,collider->GetOBB().center.y + collider->GetWorldPosition().y,collider->GetOBB().center.z + collider->GetWorldPosition().z},
			.orientations{
				 {Vector3{collider->GetWorldTransform().matWorld_.m[0][0],collider->GetWorldTransform().matWorld_.m[0][1],collider->GetWorldTransform().matWorld_.m[0][2]}},
				 {Vector3{collider->GetWorldTransform().matWorld_.m[1][0],collider->GetWorldTransform().matWorld_.m[1][1],collider->GetWorldTransform().matWorld_.m[1][2]}},
				 {Vector3{collider->GetWorldTransform().matWorld_.m[2][0],collider->GetWorldTransform().matWorld_.m[2][1],collider->GetWorldTransform().matWorld_.m[2][2]}},
			},
			.size{collider->GetOBB().size}
		};
		worldTransform_.translate += Math::PushOutAABBOBB(worldTransform_.translate, GetAABB(), collider->GetWorldTransform().translate, obb);

	}
	worldTransform_.translate.y;
	worldTransform_.UpdateMatrix();

}