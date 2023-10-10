#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "Engine.h"
#include "Sprite.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	//DirectXCommon* dxCommon_ = nullptr;
	// 3Dモデル
	std::unique_ptr<Model> model_;
	std::unique_ptr<Model> model2_;


	std::unique_ptr<Sprite> sprite_;
	std::unique_ptr<Sprite> sprite2_;


	Vector2 pos1 = {0, 0};
	Vector2 pos2 = {0, 0};


};
