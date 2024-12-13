#pragma once
#include "Framework/IScene.h"
#include "Input.h"
#include "Transition/Transition.h"

/**
 * @file ClearScene
 * @brief ゲームクリアシーンを管理するクラス
 */
class ClearScene : public IScene {
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
	std::unique_ptr<Sprite> spriteClear_;
	std::unique_ptr<Sprite> spritePushA_;
	// フェードイン・フェードアウト用
	std::unique_ptr<Transition> transition_;

};