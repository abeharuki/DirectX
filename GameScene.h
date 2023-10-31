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
	ViewProjection viewProjection_;
	WorldTransform worldTransform_;
	WorldTransform worldTransformp_;
	WorldTransform worldTransformFence_[2];
	//球
	std::unique_ptr<Sphere> sphere_;

	// 天球
	std::unique_ptr<Skydome> skydome_;
	// 天球3Dモデル
	std::unique_ptr<Model> modelSkydome_;
	//板ポリ
	std::unique_ptr<Model> modelplane_;
	//フェンス
	std::unique_ptr<Model> modelFence_[2];
	//板ポリ
	std::unique_ptr<Particle> particle_;

	std::unique_ptr<Sprite> sprite_;

	Transform uvTransform_;
	Vector4 colorPlane;
	BlendMode blendMode_;
	const char* EnumToString(BlendMode value) {
		switch (value) {
		case BlendMode::kNone:
			return "kNone"; 
		case BlendMode::kNormal:
			return "kNormal";
		case BlendMode::kAdd:
			return "kAdd";
		case BlendMode::kSubtract:
			return "kSubtract";
		default:
			return "Unknown";
		}
	}
};
