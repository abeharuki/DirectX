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
	Model* model_ = nullptr;
	Sprite* sprite_ = nullptr;
};
