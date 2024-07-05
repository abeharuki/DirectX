#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include <memory>
#include "Audio/Audio.h"
#include <Sprite.h>
#include <Model.h>
#include <Ground.h>
#include <Skydome.h>
#include <camera/followCamera.h>
#include <Player/PlayerManager.h>
#include <Enemy/EnemyManager.h>
#include <Healer/HealerManager.h>
#include <Tank/TankManager.h>
#include <Renju/RenjuManager.h>
#include <Animation/Animation.h>
#include "PostEffects/PostEffect.h"
#include <Line.h>


class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void RenderDirect() override;
	void Fade();

	void cameraMove();

private: // 基本変数
	// 光の数値
	DirectionLight directionLight_{
		{1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, -2.0f, 0.0f},
		1.0f,
		true,
	};

private:
	Audio* audio_ = nullptr;
	uint32_t audioData_[10];
	std::unique_ptr<Sprite> spriteTitle_;
	std::unique_ptr<Sprite> spritePushA_;
	//std::unique_ptr<Sprite> spriteRule_;

	// フェードイン・フェードアウト用スプライト
	std::unique_ptr<Sprite> spriteBack_;

	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	// 天球
	std::unique_ptr<Skydome> skydome_;
	// 天球3Dモデル
	std::unique_ptr<Model> modelSkydome_;
	// 地面
	std::unique_ptr<Ground> ground_;
	// 地面3Dモデル
	std::unique_ptr<Model> modelGround_;

	// 追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	// プレイヤー
	std::unique_ptr<PlayerManager> playerManager_;
	// 敵
	std::unique_ptr<EnemyManager> enemyManager_;
	//タンク
	std::unique_ptr<TankManager> tankManager_;
	// ヒーラー
	std::unique_ptr<HealerManager> healerManager_;
	// レンジャー
	std::unique_ptr<RenjuManager> renjuManager_;

	std::unique_ptr<Animations>animation_;

	Vector2 pos_;
	bool rule_;

	bool isFadeOut_;
	bool isFadeIn_;
	bool isFede_;
	float alpha_;

	float a_;
};