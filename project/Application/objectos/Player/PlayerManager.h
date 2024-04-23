#pragma once
#include "Player.h"
#include "Model.h"
#include <Particle.h>
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
	std::unique_ptr<Animations>animation_;

	std::unique_ptr<Model> Model_;
	std::unique_ptr<Model> HammerModel_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Model> bulletModel_;

	std::unique_ptr<Sprite> spriteHP_[6];
	std::unique_ptr<Sprite> spriteHPG_[6];
	std::unique_ptr<Sprite> spriteRevival_;
	std::unique_ptr<Sprite> spriteRevivalG_;
	
	Transform revivalTransform_;
	
	//パーティクル
	std::unique_ptr<Particle> particle_;
	Emitter emitter_{};
	bool isParticle_ = false;

	Transform HpTransform_;

	bool isDead_ = false;
	int revivalCount_;

	int hitCount_;

	bool preHitH_;
	bool isHitH_;

	bool preHitR_;
	bool isHitR_;

	bool preHitT_;
	bool isHitT_;
};