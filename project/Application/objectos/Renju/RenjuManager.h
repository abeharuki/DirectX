#pragma once
#include "Model.h"
#include "Renju.h"
#include <ParticleSystem.h>
#include "RenjuParticle.h"

class RenjuManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	void DrawUI();
	void RenderDirect(const ViewProjection& camera);
	Renju* GetRenju() { return renju_.get(); };
	const WorldTransform& GetWorldTransform();
	void SetViewProjection(const ViewProjection& viewProjection);

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	// 弾リストの取得
	const std::list<RenjuBullet*>& GetBullets() const { return renju_->GetBullets(); }

	void Dissolve() {
		threshold_ += 0.004f;
	}

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);
	void OnCollision(const WorldTransform& worldTransform);
	void SetParticlePos(Vector3 pos);

private:
	std::unique_ptr<Model> Model_;
	std::unique_ptr<Renju> renju_;

	std::unique_ptr<Sprite> spriteHP_;
	std::unique_ptr<Sprite> spriteHPG_;
	std::unique_ptr<Sprite> spriteMP_;
	std::unique_ptr<Sprite> spriteMPG_;
	std::unique_ptr<Sprite> spriteH_;
	std::unique_ptr<Sprite> spriteM_;
	std::unique_ptr<Sprite> sprite3P_;
	std::unique_ptr<Sprite> spriteName_;

	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_;
	bool isParticle_ = false;

	float threshold_ = 0.0f;
	Vector3 playerPos_;

	Vector2 spriteHpSize_;
	Vector2 spriteMpSize_;

	Vector4 hpColor_;

	// 味方の判定
	bool preHit_;
	bool isHit_;

	// 敵の判定
	bool preHitE_;
	bool isHitE_;
};
