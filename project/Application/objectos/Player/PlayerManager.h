#pragma once
#include "Player.h"
#include "Model.h"
#include <ParticleSystem.h>
#include <Animation/Animation.h>

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


	void Revival() { 

		if (player_->GetIsDead()) {
			if (Input::GetInstance()->GetPadConnect()) {
				if (Input::GetInstance()->GetPadButton(XINPUT_GAMEPAD_B)) {
					//復活時間
					revivalCount_++;
				}
				else {
					revivalCount_--;
				}
			}
			else {
				if (Input::GetInstance()->PressKey(DIK_B)) {
					//復活時間
					revivalCount_++;
				}
				else {
					revivalCount_--;
				}
			}


		}
		else {
			revivalCount_--;
		}

		if (revivalCount_ <= 0) {
			revivalCount_ = 0;
		}



		//revivalCount_ = player_->GetRevival();

		revivalTransform_.scale.x = revivalCount_*2.0f;
	}


private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHammer_;
	WorldTransform worldTransformShadow_;
	std::unique_ptr<Animations>animation_;

	std::unique_ptr<Model> HammerModel_;//武器
	std::unique_ptr<Model> shadowModel_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> bulletModel_;

	/*---------------UI--------------*/
	std::unique_ptr<Sprite> spriteHP_;
	std::unique_ptr<Sprite> spriteHPG_;
	std::unique_ptr<Sprite> spriteMP_;
	std::unique_ptr<Sprite> spriteMPG_;
	std::unique_ptr<Sprite> spriteH_;
	std::unique_ptr<Sprite> spriteM_;
	std::unique_ptr<Sprite> sprite1P_;
	std::unique_ptr<Sprite> spriteName_;

	std::unique_ptr<Sprite> HPnum_[3];
	std::unique_ptr<Sprite> MPnum_[3];

	std::unique_ptr<Sprite> spriteRevival_;
	std::unique_ptr<Sprite> spriteRevivalG_;
	
	Transform revivalTransform_;
	
	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

	//パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_{};
	bool isParticle_ = false;

	Vector2 spriteHpSize_ = {};
	Vector2 spriteMpSize_ = {};

	Vector4 hpColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 hpNumColor_;

	bool isDead_ = false;
	int revivalCount_;

	bool preHitH_;
	bool isHitH_;

	bool preHitR_;
	bool isHitR_;

	bool preHitT_;
	bool isHitT_;
};