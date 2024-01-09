#pragma once
#include "Player.h"
#include "Model.h"

class PlayerManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	void DrawUI();

	const WorldTransform& GetWorldTransform();
	void SetViewProjection(const ViewProjection* viewProjection);

	Vector3 GetWorldPos();
	Vector3 katanaPos();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);
	void OnCollision(const WorldTransform& worldTransform);


	bool IsAttack() { return player_->IsAttack(); }

private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHammer_;
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Model> HammerModel_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> bulletModel_;

	std::unique_ptr<Sprite> spriteHP0_;
	std::unique_ptr<Sprite> spriteHPG0_;
	std::unique_ptr<Sprite> spriteHP1_;
	std::unique_ptr<Sprite> spriteHPG1_;
	std::unique_ptr<Sprite> spriteHP2_;
	std::unique_ptr<Sprite> spriteHPG2_;
	std::unique_ptr<Sprite> spriteHP3_;
	std::unique_ptr<Sprite> spriteHPG3_;
	std::unique_ptr<Sprite> spriteHP4_;
	std::unique_ptr<Sprite> spriteHPG4_;
	std::unique_ptr<Sprite> spriteHP5_;
	std::unique_ptr<Sprite> spriteHPG5_;

	Transform HpTransform_;

	bool isDead_ = false;

	bool preHit_;
	bool isHit_;
};