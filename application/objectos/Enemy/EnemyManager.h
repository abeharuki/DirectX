#pragma once
#include "Enemy.h"
#include "Model.h"

class EnemyManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	void DrawUI();

	const WorldTransform& GetWorldTransform();
	Vector3 GetWorldPos();
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();
	void OnHealerCollision();
	void OnTankCollision();
	void OnRenjuCollision();

private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransform_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Model> bulletModel_;
	std::unique_ptr<Enemy> enemy_;

	
	std::unique_ptr<Sprite> spriteHP_;
	std::unique_ptr<Sprite> spriteHPG_;
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