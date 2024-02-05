#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "BaseCharacter.h"
#include "CollisionManager/Collider.h"

/// <summary>
/// ゲームシーン
/// </summary>
class Player : public Collider {

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
	void Initialize(const std::vector<Model*>& models);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewprojection, bool light);


	//Collider関連
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; };
	const Vector3 GetWorldPosition() const override;
	void OnCollision(Collider* collider) override;

	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

private: // メンバ変数
	WorldTransform worldTransformBase_;
	ViewProjection viewProjection_;
	
	// モデル配列データ
	std::vector<Model*> models_;
};
