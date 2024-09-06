#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <list>
#include <CollisionManager/ColliderManager.h>

class Stage : public Collider {
public:
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
	void Draw(ViewProjection viewprojection, bool light);

	void OnCollision(Collider* collider) override;
	const Vector3 GetWorldPosition() const override;
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }

	void SetLight(DirectionLight directionLight) { 
		for (int i = 0; i < 21; ++i) {
			model_[i]->DirectionalLightDraw(directionLight);
		}

	}

private:
	WorldTransform worldTransformBase_;
	WorldTransform worldTransform_[21];
	std::unique_ptr<ColliderManager> colliderManager_[21] = {};


	std::unique_ptr<Model> model_[21];
};