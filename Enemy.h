#pragma once
#include "BaseCharacter.h"
#include "DirectXCommon.h"
#include "Engine.h"
#include "KeyInput.h"
#include "Model.h"
#include "Sprite.h"
#include "WorldTransform.h"

class Enemy : public BaseCharacter {
public:
	/*Enemy();
	~Enemy();*/
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

	//void Move();

	//// ワールド座標
	//Vector3 GetWorldPosition();
	//// ローカル座標
	//Vector3 GetLocalPosition();

	const WorldTransform& GetWorldTransform() { return worldtransformBase_; }

private:
	WorldTransform worldtransformBase_;

};

