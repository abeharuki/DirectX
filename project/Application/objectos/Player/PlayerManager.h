#pragma once
#include "Player.h"
#include "Model.h"
#include <Particle.h>

class PlayerManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	void DrawUI();
	Player* GetPlayer() { return player_.get(); };
	const WorldTransform& GetWorldTransform();
	void SetViewProjection(const ViewProjection* viewProjection);


	Vector3 katanaPos();

	// 衝突を検出したら呼び出されるコールバック関数

	void OnHCollision();
	void OnRCollision();
	void OnTCollision();
	void OnCollision();
	void SetParticlePos(Vector3 pos);

	bool IsAttack() { return player_->IsAttack(); }
	bool IsOver() { return player_->IsOver(); }

private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHammer_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Model> HammerModel_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> bulletModel_;

	std::unique_ptr<Sprite> spriteHP_[6];
	std::unique_ptr<Sprite> spriteHPG_[6];

	//パーティクル
	std::unique_ptr<Particle> particle_;
	Emitter emitter_{};
	bool isParticle_ = false;
	
	Transform HpTransform_;

	bool isDead_ = false;

	int hitCount_;

	bool preHitH_;
	bool isHitH_;

	bool preHitR_;
	bool isHitR_;

	bool preHitT_;
	bool isHitT_;
};