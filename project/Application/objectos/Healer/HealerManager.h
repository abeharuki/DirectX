#pragma once
#include "Model.h"
#include "Healer.h"
#include <ParticleSystem.h>

class HealerManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	void DrawUI();
	void RenderDirect(const ViewProjection& camera);
	Healer* GetHealer() { return healer_.get(); };
	const WorldTransform& GetWorldTransform();
	void SetViewProjection(const ViewProjection& viewProjection);


	Vector3 GetCanePos() { return healer_->GetWorldTransformCane().GetWorldPos(); }

	bool IsAttack() { return healer_->IsAttack(); }

	//プレイヤーに追従
	//  プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);

	void SetParticlePos(Vector3 pos);

	void Dissolve() {
		threshold_ += 0.004f;
		
	}

private:
	
	std::unique_ptr<Model> StaffModel_;
	std::unique_ptr<Healer> healer_;

	std::unique_ptr<Sprite> spriteHP_;
	std::unique_ptr<Sprite> spriteHPG_;
	std::unique_ptr<Sprite> spriteMP_;
	std::unique_ptr<Sprite> spriteMPG_;
	std::unique_ptr<Sprite> spriteH_;
	std::unique_ptr<Sprite> spriteM_;
	std::unique_ptr<Sprite> sprite4P_;
	std::unique_ptr<Sprite> spriteName_;

	std::unique_ptr<Sprite> HPnum_[3];
	std::unique_ptr<Sprite> MPnum_[3];

	//パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_;
	bool isParticle_ = false;

	bool isDead_ = false;
	float threshold_ = 0.0f;

	Vector3 playerPos_;
	
	Vector2 spriteHpSize_;
	Vector2 spriteMpSize_;

	Vector4 hpColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 hpNumColor_;

	// 味方の判定
	bool preHit_;
	bool isHit_;

	// 敵の判定
	bool preHitE_;
	bool isHitE_;
};