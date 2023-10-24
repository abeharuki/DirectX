#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "BaseCharacter.h"

/// <summary>
/// ゲームシーン
/// </summary>
class Player : public BaseCharacter {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::vector<Model*>& models) override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewprojection, bool light) override;


	Vector3 GetWorldPosition();

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

private: // メンバ変数
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
	// 3Dモデル
	std::unique_ptr<Model> model_;

};
