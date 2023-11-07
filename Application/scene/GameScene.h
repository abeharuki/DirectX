#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "DirectXCommon.h"
#include "Model.h"
#include "Engine.h"
#include "Sprite.h"
#include "KeyInput.h"
#include "Skydome.h"
#include "Sphere.h"
#include "Particle.h"

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

	static GameScene* GetInstance();

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

private://基本変数
	//光の数値
	Vector4 color_ = {1.0f,1.0f,1.0f,1.0};
	Vector3 direction_ = {0.0f,-2.0f,0.0f};
	float intensity_ = 1.0f;

private: // メンバ変数
	

};
