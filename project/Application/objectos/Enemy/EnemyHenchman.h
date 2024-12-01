#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "Utilities/RandomGenerator.h"
#include "CollisionManager/Collider.h"
#include <Animation/Animation.h>
#include <CollisionManager/ColliderManager.h>
#include "Editor/editor.h"
#include <AnimationManager.h>



class EnemyHenchman : public Collider {
public:
	~EnemyHenchman();

	void Init(Animations* animation,Vector3 pos);
	void Update();
	void Draw(const ViewProjection& camera);

	void followRenju();
	void SetRenjuPos(Vector3 pos) { renjuPos_ = pos; }
	bool IsDead() { return dead_; }
	bool IsHitEnemy() { return enemyHit_; }

	//当たりは判定
	void OnCollision(Collider* collider) override;
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransform_; }
	Vector3 GetPos() {return worldTransform_.translate;}

private:
	WorldTransform worldTransform_;
	Animations* animation_;

	bool dead_;
	Vector3 renjuPos_;
	Vector3 velocity_;

	int time_ = 60 * 4;

	float destinationAngleY_;
	bool hit_;
	bool enemyHit_;
};


