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



class TutorialScene : public IScene {
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
	std::unique_ptr<Sprite> spriteStep1_;
	std::unique_ptr<Sprite> spriteCombo_;
	std::unique_ptr<Sprite> spriteStep2_;
	std::unique_ptr<Sprite> spriteTu_;
	std::unique_ptr<Sprite> spriteSta_;
	std::unique_ptr<Sprite> spriteSen_[7];

	// フェードイン・フェードアウト用スプライト
	std::unique_ptr<Sprite> spriteBack_;


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



	Vector2 pos_;
	float size_[7];
	bool rule_;

	bool isFadeOut_;
	bool isFadeIn_;
	bool isFede_;
	float alpha_;

	Vector3 initialPos_;

	//チュートリアル用フラグ
	//ステップ1
	bool move_;
	bool dash_;
	bool jump_;
	bool attack_;
	bool combo_;

	//ステップ2
	bool step2_;
	bool call_;//仲間を呼ぶ
	bool order_;//攻撃命令
};
