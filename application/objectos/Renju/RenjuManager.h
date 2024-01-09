#pragma once
#include "Model.h"
#include "Renju.h"

class RenjuManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);

	const WorldTransform& GetWorldTransform();
	void SetViewProjection(const ViewProjection& viewProjection);
	Vector3 GetWorldPos() { return renju_->GetWorldTransform().GetWorldPos(); }

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	//敵の位置を検索
	void SetEnemypPos(Vector3 enemyPos) { enemyPos_ = enemyPos; };

	// 弾リストの取得
	const std::list<RenjuBullet*>& GetBullets() const { return renju_->GetBullets(); }

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);

	
private:
	WorldTransform worldTransformBase_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Renju> renju_;

	bool isDead_ = false;
	Vector3 playerPos_;
	Vector3 enemyPos_;

	// 味方の判定
	bool preHit_;
	bool isHit_;
};