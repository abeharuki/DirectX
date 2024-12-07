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


/**
 * @file EnemyHenchman.h
 * @brief 敵が出す子分キャラクターを制御するクラス
 */
class EnemyHenchman : public Collider {
public:
	~EnemyHenchman();

	void Init(Animations* animation,Vector3 pos);
	void Update();
	void Draw(const ViewProjection& camera);

	//レンジャーに向かって動く
	void followRenju();

	//当たりは判定
	void OnCollision(Collider* collider) override;

	/*-----------ゲッター-----------*/
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransform_; }
	Vector3 GetPos() { return worldTransform_.translate; }
	bool IsDead() { return dead_; }
	bool IsHitEnemy() { return enemyHit_; }

	/*------------セッター---------*/
	void SetRenjuPos(Vector3 pos) { renjuPos_ = pos; }
	void SetSpacal(bool flag) { specal_ = flag; }

private:
	WorldTransform worldTransform_;
	Animations* animation_;

	
	Vector3 renjuPos_;
	Vector3 velocity_;

	//対象の方を向くための角度
	float destinationAngleY_;
	//ヒット確認
	bool hit_;
	bool enemyHit_;

	//必殺技フラグ受け取り用
	bool specal_;

	//ディゾルブ用
	float thre_ = 0.0f;

	//死亡フラグ
	bool dead_;
};


