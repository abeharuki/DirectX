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
	

	/*-----ゲッター-----*/
	Healer* GetHealer() { return healer_.get(); };
	Vector3 GetCanePos() { return healer_->GetWorldTransformCane().GetWorldPos(); }

	bool IsAttack() { return healer_->IsAttack(); }

	/*-----セッター-----*/
	void SetPlayerPos(Vector3 playerPos) { playerPos_ = playerPos; };
	void SetViewProjection(const ViewProjection& camera) { healer_->SetViewProjection(camera); };
	//パーティクルのpos
	void SetParticlePos(Vector3 pos);

private:
	WorldTransform worldTransformShadow_;
	//アニメーション
	std::unique_ptr<Animations>animation_;
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

	Vector3 playerPos_;
	
	Vector2 spriteHpSize_;
	Vector2 spriteMpSize_;

	Vector4 hpColor_ = { 1.0f,1.0f,1.0f,1.0f };//hp文字の色
	Vector4 hpNumColor_;//hpの数字の色

	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

	
};