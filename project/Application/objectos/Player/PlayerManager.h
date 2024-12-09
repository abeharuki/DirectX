#pragma once
#include "Player.h"
#include "Model.h"
#include <ParticleSystem.h>
#include <Animation/Animation.h>

/**
 * @file PlayerManager
 * @brief プレイヤーが操作するキャラクターを管理するクラス
 */
class PlayerManager {
public:
	//キャラクターの生成
	void Initialize();
	//キャラクターの更新
	void Update();
	//キャラクターの描画
	void Draw(const ViewProjection& camera);
	//キャラクターUIの描画
	void DrawUI();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnHCollision();
	void OnRCollision();
	void OnTCollision();


	//復活(今は使ってない)
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

	/*-----ゲッター-----*/
	Player* GetPlayer() { return player_.get(); };
	const WorldTransform& GetWorldTransform();
	Vector3 katanaPos();

	bool IsAttack() { return player_->IsAttack(); }
	bool IsOver() { return player_->IsOver(); }

	/*------セッター-----*/
	void SetViewProjection(const ViewProjection* viewProjection);
	void SetParticlePos(Vector3 pos);

private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformHammer_;
	WorldTransform worldTransformShadow_;


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

	std::unique_ptr<Sprite> HPnum_[3];//数字
	std::unique_ptr<Sprite> MPnum_[3];//数字

	std::unique_ptr<Sprite> spriteRevival_;
	std::unique_ptr<Sprite> spriteRevivalG_;
	
	Transform revivalTransform_;
	
	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

	//パーティクル
	ParticleSystem* particle_;
	EmitterSphere emitter_{};
	bool isParticle_ = false;

	Vector2 spriteHpSize_ = {};
	Vector2 spriteMpSize_ = {};

	Vector4 hpColor_ = { 1.0f,1.0f,1.0f,1.0f };//hp文字の色
	Vector4 hpNumColor_;//hp数字の色

	bool isDead_ = false;
	int revivalCount_;//復活までの時間

	bool preHitH_;
	bool isHitH_;

	bool preHitR_;
	bool isHitR_;

	bool preHitT_;
	bool isHitT_;
};