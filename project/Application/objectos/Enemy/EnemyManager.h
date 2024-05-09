#pragma once
#include "Enemy.h"
#include "Model.h"
#include <Sphere.h>


class EnemyManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	Enemy* GetEnemy() { return enemy_.get(); };
	void DrawUI();

	bool IsAttack() { return enemy_->isAttack(); }
	bool IsClear() { return enemy_->isClear(); }

	void SetPlayerPos(Vector3 pos) { enemy_->SetPlayerPos(pos); };
	void SetHealerPos(Vector3 pos) { enemy_->SetHealerPos(pos); };
	void SetRenjuPos(Vector3 pos) { enemy_->SetRenjuPos(pos); };
	void SetTankPos(Vector3 pos) { enemy_->SetTankPos(pos); };

	const WorldTransform& GetWorldTransform();
	Vector3 GetRockWorldPos();
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();
	void OnHealerCollision();
	void OnTankCollision();
	void OnRenjuCollision();

private:
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Sphere> rockModel_;
	std::unique_ptr<Model> bulletModel_;
	std::unique_ptr<Enemy> enemy_;


	std::unique_ptr<Sprite> spriteHP_;
	std::unique_ptr<Sprite> spriteHPG_;
	std::unique_ptr<Sprite> spriteBoss_;
	Transform HpTransform_;

	bool isDead_ = false;


	bool preHit_;
	bool isHit_;

	bool preHitH_;
	bool isHitH_;

	bool preHitR_;
	bool isHitR_;

	bool preHitT_;
	bool isHitT_;

};
