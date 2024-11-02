#include "CollisionManager.h"
#include "CollisionConfig.h"
#include <algorithm>

void CollisionManager::ClearColliderList()
{
	//コライダーリストをクリア
	colliders_.clear();
}

void CollisionManager::SetColliderList(Collider* collider)
{
	collider->CheckCollision(false);
	//コライダーリストに登録
	colliders_.push_back(collider);
}

void CollisionManager::CheckAllCollisions()
{


	//リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{
		//イテレータAからコライダーAを取得する
		Collider* colliderA = *itrA;
		//イテレータBはイテレータAの次の要素から回す(重複判定を回避)
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB)
		{
			//イテレータBからコライダーBを取得する
			Collider* colliderB = *itrB;
			//ベアの当たり判定

			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{


	//衝突フィルタリング
	if ((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0 ||
		(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0) {

		return;
	}

	//球と球の判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0) ||
		((colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0 && (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0))
	{
		//コライダーAのワールド座標を取得
		Vector3 posA = colliderA->GetWorldPosition();
		//コライダーBのワールド座標を取得
		Vector3 posB = colliderB->GetWorldPosition();
		//コライダーAのSphereを作成
		sphere sphereA = { .center{posA},.radius{colliderA->GetRadius()} };
		//コライダーBのSphereを作成
		sphere sphereB = { .center{posB},.radius{colliderB->GetRadius()} };
		//衝突判定
		if (CheckCollisionSphere(sphereA, sphereB))
		{
			//コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB);
			//コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderA);
			colliderA->CheckCollision(true);
			colliderB->CheckCollision(true);
		}
	}

	//AABBとAABBの判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0) ||
		((colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0))
	{
		//コライダーAのワールド座標を取得
		Vector3 posA = colliderA->GetWorldPosition();
		//コライダーBのワールド座標を取得
		Vector3 posB = colliderB->GetWorldPosition();
		//コライダーAのAABBを取得
		AABB aabbA = { .min{posA + colliderA->GetAABB().min},.max{posA + colliderA->GetAABB().max} };
		//コライダーBのAABBを取得
		AABB aabbB = { .min{posB + colliderB->GetAABB().min},.max{posB + colliderB->GetAABB().max} };
		//衝突判定
		if (CheckCollisionAABB(aabbA, aabbB))
		{
			//コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB);
			//コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderA);
			colliderA->CheckCollision(true);
			colliderB->CheckCollision(true);
		}
	}

	//球とAABBの判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0) ||
		((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0))
	{
		//コライダーAのワールド座標を取得
		Vector3 posA = colliderA->GetWorldPosition();
		//コライダーBのワールド座標を取得
		Vector3 posB = colliderB->GetWorldPosition();

		//コライダーAがSphereの場合
		if (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere)
		{
			//コライダーAのSphereを作成
			sphere sphere = { .center{posA},.radius{colliderA->GetRadius()} };
			//コライダーBのAABBを取得
			AABB aabb = { .min{posB + colliderB->GetAABB().min},.max{posB + colliderB->GetAABB().max} };
			//衝突判定
			if (CheckCollisionSphereAABB(sphere, aabb))
			{
				//コライダーAの衝突時コールバックを呼び出す
				colliderA->OnCollision(colliderB);
				//コライダーBの衝突時コールバックを呼び出す
				colliderB->OnCollision(colliderA);
				colliderA->CheckCollision(true);
				colliderB->CheckCollision(true);
			}
		}
		else if (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere)
		{
			//コライダーAのSphereを作成
			sphere sphere = { .center{posB},.radius{colliderB->GetRadius()} };
			//コライダーBのAABBを取得
			AABB aabb = { .min{posA + colliderA->GetAABB().min},.max{posA + colliderA->GetAABB().max} };
			//衝突判定
			if (CheckCollisionSphereAABB(sphere, aabb))
			{
				//コライダーAの衝突時コールバックを呼び出す
				colliderA->OnCollision(colliderB);
				//コライダーBの衝突時コールバックを呼び出す
				colliderB->OnCollision(colliderA);
			}
		}
	}

	//OBBとAABBの判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveOBB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0) ||
		((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveOBB) != 0))
	{
		//コライダーAがAABBの場合
		if (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB)
		{
			//コライダーAのAABBを取得
			AABB aabb = { .min{colliderA->GetWorldPosition() + colliderA->GetAABB().min},.max{colliderA->GetWorldPosition() + colliderA->GetAABB().max}, };

			//コライダーBのOBBを取得
			OBB obb = {
				.center{colliderB->GetOBB().center.x + colliderB->GetWorldPosition().x,colliderB->GetOBB().center.y + colliderB->GetWorldPosition().y,colliderB->GetOBB().center.z + colliderB->GetWorldPosition().z},
				.orientations{
				 {Vector3{colliderB->GetWorldTransform().matWorld_.m[0][0],colliderB->GetWorldTransform().matWorld_.m[0][1],colliderB->GetWorldTransform().matWorld_.m[0][2]}},
				 {Vector3{colliderB->GetWorldTransform().matWorld_.m[1][0],colliderB->GetWorldTransform().matWorld_.m[1][1],colliderB->GetWorldTransform().matWorld_.m[1][2]}},
				 {Vector3{colliderB->GetWorldTransform().matWorld_.m[2][0],colliderB->GetWorldTransform().matWorld_.m[2][1],colliderB->GetWorldTransform().matWorld_.m[2][2]}}

				},
				.size{colliderB->GetOBB().size}
			};



			//衝突判定
			if (CheckCollisionAABBOBB(aabb, obb))
			{
				//コライダーAの衝突時コールバックを呼び出す
				colliderA->OnCollision(colliderB);
				//コライダーBの衝突時コールバックを呼び出す
				colliderB->OnCollision(colliderA);
				colliderA->CheckCollision(true);
				colliderB->CheckCollision(true);
			}
		}
		//ColliderBがAABBの場合
		else if (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) {
			//コライダーBのAABBを取得
			AABB aabb = { .min{colliderB->GetWorldPosition() + colliderB->GetAABB().min},.max{colliderB->GetWorldPosition() + colliderB->GetAABB().max}, };
			//コライダーAのOBBを取得

			//コライダーAのOBBを取得
			OBB obb = {
				.center{colliderA->GetOBB().center.x + colliderA->GetWorldPosition().x,colliderA->GetOBB().center.y + colliderA->GetWorldPosition().y,colliderA->GetOBB().center.z + colliderA->GetWorldPosition().z},

				.orientations{
				 {Vector3{colliderA->GetWorldTransform().matWorld_.m[0][0],colliderA->GetWorldTransform().matWorld_.m[0][1],colliderA->GetWorldTransform().matWorld_.m[0][2]}},
				 {Vector3{colliderA->GetWorldTransform().matWorld_.m[1][0],colliderA->GetWorldTransform().matWorld_.m[1][1],colliderA->GetWorldTransform().matWorld_.m[1][2]}},
				 {Vector3{colliderA->GetWorldTransform().matWorld_.m[2][0],colliderA->GetWorldTransform().matWorld_.m[2][1],colliderA->GetWorldTransform().matWorld_.m[2][2]}},
				},
				.size{colliderA->GetOBB().size}
			};





			//衝突判定
			if (CheckCollisionAABBOBB(aabb, obb))
			{
				//コライダーAの衝突時コールバックを呼び出す
				colliderA->OnCollision(colliderB);
				//コライダーBの衝突時コールバックを呼び出す
				colliderB->OnCollision(colliderA);
				colliderA->CheckCollision(true);
				colliderB->CheckCollision(true);
			}
		}
	}

	//OBBとOBBの判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveOBB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveOBB) != 0) ||
		((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveOBB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveOBB) != 0))
	{
		//コライダーAのAABBを取得
		OBB obbA = {
				.center{colliderA->GetOBB().center.x + colliderA->GetWorldPosition().x,colliderA->GetOBB().center.y + colliderA->GetWorldPosition().y,colliderA->GetOBB().center.z + colliderA->GetWorldPosition().z},

				.orientations{
				 {Vector3{colliderA->GetWorldTransform().matWorld_.m[0][0],colliderA->GetWorldTransform().matWorld_.m[0][1],colliderA->GetWorldTransform().matWorld_.m[0][2]}},
				 {Vector3{colliderA->GetWorldTransform().matWorld_.m[1][0],colliderA->GetWorldTransform().matWorld_.m[1][1],colliderA->GetWorldTransform().matWorld_.m[1][2]}},
				 {Vector3{colliderA->GetWorldTransform().matWorld_.m[2][0],colliderA->GetWorldTransform().matWorld_.m[2][1],colliderA->GetWorldTransform().matWorld_.m[2][2]}},
				},
				.size{colliderA->GetOBB().size}
		};
		//コライダーBのOBBを取得
		OBB obbB = {
				.center{colliderB->GetOBB().center.x + colliderB->GetWorldPosition().x,colliderB->GetOBB().center.y + colliderB->GetWorldPosition().y,colliderB->GetOBB().center.z + colliderB->GetWorldPosition().z},
				.orientations{
				 {Vector3{colliderB->GetWorldTransform().matWorld_.m[0][0],colliderB->GetWorldTransform().matWorld_.m[0][1],colliderB->GetWorldTransform().matWorld_.m[0][2]}},
				 {Vector3{colliderB->GetWorldTransform().matWorld_.m[1][0],colliderB->GetWorldTransform().matWorld_.m[1][1],colliderB->GetWorldTransform().matWorld_.m[1][2]}},
				 {Vector3{colliderB->GetWorldTransform().matWorld_.m[2][0],colliderB->GetWorldTransform().matWorld_.m[2][1],colliderB->GetWorldTransform().matWorld_.m[2][2]}}

				},
				.size{colliderB->GetOBB().size}
		};

		//衝突判定
		if (CheckCollisionOBB(obbA, obbB))
		{
			//コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB);
			//コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderA);
			colliderA->CheckCollision(true);
			colliderB->CheckCollision(true);
		}

	}

}

bool CollisionManager::CheckCollisionSphere(const sphere& sphereA, const sphere& sphereB)
{
	//コライダーAとコライダーBの距離を計算
	float distance = Math::Length(sphereA.center - sphereB.center);
	//球と球の交差判定
	if (distance <= sphereA.radius + sphereB.radius)
	{
		return true;
	}
	return false;
}

bool CollisionManager::CheckCollisionSphereAABB(const sphere& sphere, const AABB& aabb)
{
	//最近接点を求める
	Vector3 closestPoint{
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z) };
	//最近接点と球の中心との距離を求める
	float distance = Math::Length(closestPoint - sphere.center);
	//距離が半径よりも小さければ衝突
	if (distance <= sphere.radius)
	{
		return true;
	}
	return false;
}

bool CollisionManager::CheckCollisionAABB(const AABB& aabbA, const AABB& aabbB)
{
	if (aabbA.min.x <= aabbB.max.x && aabbA.max.x >= aabbB.min.x &&
		aabbA.min.y <= aabbB.max.y && aabbA.max.y >= aabbB.min.y &&
		aabbA.min.z <= aabbB.max.z && aabbA.max.z >= aabbB.min.z)
	{
		return true;
	}
	return false;
}

bool CollisionManager::CheckCollisionAABBOBB(const AABB& aabb, const OBB& obb)
{
	Vector3 aabbCenter = (aabb.min + aabb.max) * 0.5f;

	float aabbHalfSize[3] = {
	0.5f * (aabb.max.x - aabb.min.x),
	0.5f * (aabb.max.y - aabb.min.y),
	0.5f * (aabb.max.z - aabb.min.z),
	};

	Vector3 aabbAxis[3] = {
		{1.0f,0.0f,0.0f},
		{0.0f,1.0f,0.0f},
		{0.0f,0.0f,1.0f}
	};

	float obbHalfSize[3] = {
		obb.size.x,
		obb.size.y,
		obb.size.z
	};

	Vector3 obbAxis[3] = {
		obb.orientations[0],
		obb.orientations[1],
		obb.orientations[2],
	};

	float t[3] = {
		obb.center.x - aabbCenter.x,
		obb.center.y - aabbCenter.y,
		obb.center.z - aabbCenter.z,
	};

	const float EPSILON = 1.175494e-37f;

	float R[3][3], AbsR[3][3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = Math::Dot(aabbAxis[i], obb.orientations[j]);
			AbsR[i][j] = fabsf(R[i][j]) + EPSILON;
		}
	}

	//軸L=A0, L=A1, L=A2判定
	float ra, rb;

	for (int i = 0; i < 3; i++)
	{
		ra = aabbHalfSize[i];
		rb = obbHalfSize[0] * AbsR[i][0] + obbHalfSize[1] * AbsR[i][1] + obbHalfSize[2] * AbsR[i][2];
		if (fabsf(t[i]) > ra + rb)return false;
	}
	//軸L=B0, L=B1, L=B2判定
	for (int i = 0; i < 3; i++)
	{
		ra = aabbHalfSize[0] * AbsR[0][i] + aabbHalfSize[1] * AbsR[1][i] + aabbHalfSize[2] * AbsR[2][i];
		rb = obbHalfSize[i];
		if (fabsf(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb)return false;
	}

	//軸L=A0 X B0判定
	ra = aabbHalfSize[1] * AbsR[2][0] + aabbHalfSize[2] * AbsR[1][0];
	rb = obbHalfSize[1] * AbsR[0][2] + obbHalfSize[2] * AbsR[0][1];
	if (fabsf(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)return false;

	//軸L=A0 X B1判定
	ra = aabbHalfSize[1] * AbsR[2][1] + aabbHalfSize[2] * AbsR[1][1];
	rb = obbHalfSize[0] * AbsR[0][2] + obbHalfSize[2] * AbsR[0][0];
	if (fabsf(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)return false;

	//軸L=A0 X B2判定
	ra = aabbHalfSize[1] * AbsR[2][2] + aabbHalfSize[2] * AbsR[1][2];
	rb = obbHalfSize[0] * AbsR[0][1] + obbHalfSize[1] * AbsR[0][0];
	if (fabsf(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)return false;

	//軸L=A1 X B0判定
	ra = aabbHalfSize[0] * AbsR[2][0] + aabbHalfSize[2] * AbsR[0][0];
	rb = obbHalfSize[1] * AbsR[1][2] + obbHalfSize[2] * AbsR[1][1];
	if (fabsf(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)return false;

	//軸L=A1 X B1判定
	ra = aabbHalfSize[0] * AbsR[2][1] + aabbHalfSize[2] * AbsR[0][1];
	rb = obbHalfSize[0] * AbsR[1][2] + obbHalfSize[2] * AbsR[1][0];
	if (fabsf(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)return false;

	//軸L=A1 X B2判定
	ra = aabbHalfSize[0] * AbsR[2][2] + aabbHalfSize[2] * AbsR[0][2];
	rb = obbHalfSize[0] * AbsR[1][1] + obbHalfSize[1] * AbsR[1][0];
	if (fabsf(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)return false;

	//軸L=A2 X B0判定
	ra = aabbHalfSize[0] * AbsR[1][0] + aabbHalfSize[1] * AbsR[0][0];
	rb = obbHalfSize[1] * AbsR[2][2] + obbHalfSize[2] * AbsR[2][1];
	if (fabsf(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)return false;

	//軸L=A2 X B1判定
	ra = aabbHalfSize[0] * AbsR[1][1] + aabbHalfSize[1] * AbsR[0][1];
	rb = obbHalfSize[0] * AbsR[2][2] + obbHalfSize[2] * AbsR[2][0];
	if (fabsf(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)return false;

	//軸L=A2 X B2判定
	ra = aabbHalfSize[0] * AbsR[1][2] + aabbHalfSize[1] * AbsR[0][2];
	rb = obbHalfSize[0] * AbsR[2][1] + obbHalfSize[1] * AbsR[2][0];
	if (fabsf(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)return false;

	return true;
}

bool CollisionManager::CheckCollisionOBB(const OBB& obbA, const OBB& obbB)
{
	Vector3 NAe1 = obbA.orientations[0];
	Vector3 Ae1 = NAe1 * obbA.size.x;
	Vector3 NAe2 = obbA.orientations[1];
	Vector3 Ae2 = NAe2 * obbA.size.y;
	Vector3 NAe3 = obbA.orientations[2];
	Vector3 Ae3 = NAe3 * obbA.size.z;

	Vector3 NBe1 = obbB.orientations[0];
	Vector3 Be1 = NBe1 * obbB.size.x;
	Vector3 NBe2 = obbB.orientations[1];
	Vector3 Be2 = NBe2 * obbB.size.y;
	Vector3 NBe3 = obbB.orientations[2];
	Vector3 Be3 = NBe3 * obbB.size.z;

	Vector3 Interval = obbA.center - obbB.center;

	auto LenSegOnSeparateAxis = [](const Vector3* Sep, const Vector3* e1, const Vector3* e2, const Vector3* e3) -> float {
		float r1 = fabs(Math::Dot(*Sep, *e1));
		float r2 = fabs(Math::Dot(*Sep, *e2));
		float r3 = e3 ? (fabs(Math::Dot(*Sep, *e3))) : 0;
		return r1 + r2 + r3;
		};

	//分理軸 Ae1
	float rA = Math::Length(Ae1);
	float rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	float L = fabs(Math::Dot(Interval, NAe1));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Ae2
	rA = Math::Length(Ae2);
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabs(Math::Dot(Interval, NAe2));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Ae3
	rA = Math::Length(Ae3);
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabs(Math::Dot(Interval, NAe3));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = Math::Length(Be1);
	L = fabs(Math::Dot(Interval, NBe1));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = Math::Length(Be2);
	L = fabs(Math::Dot(Interval, NBe2));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = Math::Length(Be3);
	L = fabs(Math::Dot(Interval, NBe3));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C11
	Vector3 Cross = Math::Cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
	L = fabs(Math::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C12
	Cross = Math::Cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
	L = fabs(Math::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C13
	Cross = Math::Cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
	L = fabs(Math::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C21
	Cross = Math::Cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
	L = fabs(Math::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C22
	Cross = Math::Cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
	L = fabs(Math::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C23
	Cross = Math::Cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
	L = fabs(Math::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C31
	Cross = Math::Cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
	L = fabs(Math::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C32
	Cross = Math::Cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
	L = fabs(Math::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C33
	Cross = Math::Cross(NAe3, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
	L = fabs(Math::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	return true;
}

