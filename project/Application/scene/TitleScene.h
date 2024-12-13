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
#include <Transition/Transition.h>

//タイトルシーンクラスの定数
namespace TitleSceneConstants {
	// スプライトのサイズ
	const Vector2 kSpriteTitleSize = { 1280.0f, 905.0f };   // タイトルスプライトのサイズ


	// タイトルスプライトの位置
	const Vector2 kSpriteTitlePos = { 0.0f, -250.0f };  // タイトルスプライトの位置
	const Vector2 kSpritePushAPos = { 50.0f, 0.0f };    // "Press A" スプライトの位置

	// その他の定数
	const float kOutLineValue = 0.005f;  // アウトラインの値
}

/**
 * @file TitleScene
 * @brief ゲームのタイトルシーンを管理するクラス
 */
class TitleScene : public IScene {
public:

	//シーンの初期化
	void Initialize() override;
	//フレームごとの更新処理
	void Update() override;
	//シーンの描画
	void Draw() override;
	//ポストエフェクトのかからないオブジェクトの描画
	void RenderDirect() override;


private:
	
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

	// フェードイン・フェードアウト用
	std::unique_ptr<Transition> transition_;
	
	std::vector<AllyAICharacter*> characters;
	//ノードエディター
	Editor::NodeEditor editor_;
};