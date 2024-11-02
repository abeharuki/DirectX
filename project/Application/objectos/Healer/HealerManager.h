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
	
	
	//プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	
	void Dissolve() {
		threshold_ += 0.004f;
		
	}

	/*-----ゲッター-----*/
	Healer* GetHealer() { return healer_.get(); };
	const WorldTransform& GetWorldTransform();
	Vector3 GetCanePos() { return healer_->GetWorldTransformCane().GetWorldPos(); }

	bool IsAttack() { return healer_->IsAttack(); }

	/*-----セッター-----*/
	void SetViewProjection(const ViewProjection& viewProjection);
	//パーティクルのpos
	void SetParticlePos(Vector3 pos);

private:
	WorldTransform worldTransformShadow_;
	std::unique_ptr<Model> shadowModel_;
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

	std::unique_ptr<Sprite> HPnum_[3];//hpの数字
	std::unique_ptr<Sprite> MPnum_[3];//mpの数字

	//パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_;
	bool isParticle_ = false;

	bool isDead_ = false;
	float threshold_ = 0.0f;//ディゾルブ

	Vector3 playerPos_;
	
	Vector2 spriteHpSize_;
	Vector2 spriteMpSize_;

	Vector4 hpColor_ = { 1.0f,1.0f,1.0f,1.0f };//hp文字の色
	Vector4 hpNumColor_;//hpの数字の色

	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

	// 味方の判定
	bool preHit_;
	bool isHit_;

	// 敵の判定
	bool preHitE_;
	bool isHitE_;
};