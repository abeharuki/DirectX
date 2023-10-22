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
	void Draw(const ViewProjection& viewprojection) override;


	void Move();


	Vector3 GetWorldPosition();

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

	//落下
	void IsFall();

	//当たり判定
	void OnCollision();
	void OutCollision();

private: // メンバ変数
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
	float upSpeed_;
	float fallSpeed_;
	bool jump_;
	bool isHit_;

	std::unique_ptr<KeyInput> input;
};
