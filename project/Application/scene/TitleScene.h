#pragma once
#include "Framework/IScene.h"
#include "Audio/Audio.h"
#include <Ground.h>
#include <Skydome.h>
#include <camera/followCamera.h>
#include <Player/PlayerManager.h>
#include <Enemy/EnemyManager.h>
#include <Healer/HealerManager.h>
#include <Tank/TankManager.h>
#include <Renju/RenjuManager.h>
#include "PostEffects/PostEffect.h"
#include <ModelLoader.h>


/**
 * @file TitleScene
 * @brief ゲームのタイトルシーンを管理するクラス
 */
class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void RenderDirect() override;


private:
	//フェードイン・フェードアウト
	void Fade();

	//カメラの動き
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
	//uint32_t audioData_[10];
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
	//ローダー
	std::unique_ptr<ModelLoader> loader_;

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
	bool rule_;

	bool isFadeOut_;
	bool isFadeIn_;
	bool isFede_;
	float alpha_;

	float a_;
	std::vector<BaseCharacter*> characters;
	//ノードエディター
	Editor::NodeEditor editor_;
};