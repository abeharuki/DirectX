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

	//床のWorldTransformを受け取る
	void SetWorldTransform(const WorldTransform& worldTransform) {
		worldTransformFloor_ = worldTransform;
	}

	//階層構造
	void Relationship();
	void Relationship(const WorldTransform& worldTransformFloor);

	//落下
	void IsFall();

	//当たり判定
	void OnCollision();
	void OutCollision();
	//移動床との当たり判定
	void OnCollisionFloor();
	void OutCollisionFloor();

private: // メンバ変数
	WorldTransform worldtransform;
	WorldTransform worldTransform_;
	WorldTransform worldTransformFloor_;
	ViewProjection viewProjection_;
	float upSpeed_;
	float fallSpeed_;
	bool jump_;
	bool isHit_;
	bool isHitFloor_;

	std::unique_ptr<KeyInput> input;
};
