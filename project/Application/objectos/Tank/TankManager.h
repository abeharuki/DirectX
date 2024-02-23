#pragma once
#include "Model.h"
#include "Tank.h"
#include <Particle.h>

class TankManager {
	public:
		void Initialize();
		void Update();
		void Draw(const ViewProjection& camera);
		Tank* GetTank() { return tank_.get(); };
		const WorldTransform& GetWorldTransform();
		void SetViewProjection(const ViewProjection& viewProjection);

		// プレイヤーに追従
		void followPlayer(Vector3 playerPos);
		// 敵の位置を検索
		void SetEnemypPos(Vector3 enemyPos) { enemyPos_ = enemyPos; };

		// 衝突を検出したら呼び出されるコールバック関数
		void OnAllyCollision(const WorldTransform& worldTransform);
		void OnCollision(const WorldTransform& worldTransform);
		void SetParticlePos(Vector3 pos);

		bool GetAttack() { return tank_->GetAttack(); }

	private:

		std::unique_ptr<Model> Model_;
		std::unique_ptr<Tank> tank_;

		std::unique_ptr<Particle> particle_;
		Emitter emitter_{};
		bool isParticle_ = false;

		bool isDead_ = false;

		Vector3 playerPos_;
		Vector3 enemyPos_;

		// 味方の判定
		bool preHit_;
		bool isHit_;

		// 敵の判定
		bool preHitE_;
		bool isHitE_;
};
