#include "RenjuBullet.h"
#include <cassert>
#include <CollisionManager/CollisionConfig.h>




/// <summary>
/// 初期化
/// </summary>
void RenjuBullet::Initialize(
	Model* model, Vector3& position, const Vector3& scale,const Vector3& rotation,
	const Vector3& velocity,bool skill) {

	assert(model);

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translate = position;
	worldTransform_.scale = scale;
	worldTransform_.rotate = rotation;

	//衝撃波の初期設定
	for (int i = 0; i < 3;++i) {
		shockwave_[i].reset(Model::CreateModelFromObj("resources/particle/scalePlane.obj", "resources/Enemy/white.png"));
		shockwave_[i]->SetMaskTexture("shockwave2.png");
		shockwave_[i]->SetBlendMode(BlendMode::kNormal);
		shockwave_[i]->IsGradient(true);
	
		
		worldTransformShockwave_[i].Initialize();
		worldTransformShockwave_[i].translate = position;
		worldTransformShockwave_[i].scale = { 0.f,0.f,0.f };//Vector3{ 5.f,5.f,5.f} - (1.5f * i);
		worldTransformShockwave_[i].rotate = { rotation.x,rotation.y,0.f};


		shockData_[i].alpha_ = 1.f;
		shockData_[i].threshold_ = 0.f;
		shockData_[i].shockVelocity_ = 0.1f;
		shockData_[i].isActive = true;
	}
	//shockData_[0].alpha_ = 0.8f;
	shockwave_[0]->SetMaskTexture("shockwave.png");

	//スキルの攻撃か如何か
	skill_ = skill;

	Draw_ = true;
	
	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;
	AABB aabbSize{ .min{-0.3f,-0.3f,-0.3f},.max{0.3f,1.0f,0.3f} };
	OBB obb;
	obb.size = { 0.5f,1.f,0.5f };
	obb.center = { 0.0f,2.0f,0.0f };
	SetOBB(obb);
	SetCollisionPrimitive(kCollisionPrimitiveOBB);
	SetCollisionAttribute(kCollisionAttributeRenju);
	SetCollisionMask(kCollisionMaskRenju);
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void RenjuBullet::Update() {

	// 時間経過でデス
	if (shockData_[2].alpha_ <= 0) {
		isDead_ = true;
	}

	shockData_[0].threshold_ += shockData_[0].shockVelocity_;
	shockData_[0].alpha_ -= 0.1f;
	shockData_[0].scaleVelocity_ = Math::Lerp(worldTransformShockwave_[0].scale, { 8.f,8.f,8.f }, 0.8f);
	

	if (shockData_[0].alpha_ <= 0.5f) {
		
		shockData_[1].threshold_ += shockData_[1].shockVelocity_;
		shockData_[1].alpha_ -= 0.1f;
		shockData_[1].scaleVelocity_ = Math::Lerp(worldTransformShockwave_[1].scale, { 5.f,5.f,5.f }, 0.4f);
	}
	if (shockData_[0].alpha_ <= 0.1f) {
		shockData_[2].threshold_ += shockData_[2].shockVelocity_;
		shockData_[2].alpha_ -= 0.1f;
		shockData_[2].scaleVelocity_ = Math::Lerp(worldTransformShockwave_[2].scale, { 5.f,5.f,5.f }, 0.4f);
	}

	worldTransform_.translate = worldTransform_.translate + velocity_;
	//衝撃波のpos
	for (int i = 0; i < 3; ++i) {
		if (shockData_[i].threshold_ >= 1.0f) {
			shockData_[i].threshold_ = 1.0f;
		}

		shockwave_[i]->SetThreshold(shockData_[i].threshold_);
		shockwave_[i]->SetColor({ 0.2f,0.f,1.f,shockData_[i].alpha_});
		worldTransformShockwave_[i].scale = shockData_[i].scaleVelocity_;
		if (worldTransformShockwave_[i].scale.x == 0) {
			worldTransformShockwave_[i].translate = worldTransform_.translate;
		}
		if (shockData_[i].alpha_ <= 0.0f) {
			shockData_[i].isActive = false;
		}
		
	}
	worldTransform_.UpdateMatrix();
	for (int i = 0; i < 3; ++i) {
		worldTransformShockwave_[i].UpdateMatrix();
	}
};

/// <summary>
/// 描画
/// </summary>
void RenjuBullet::Draw(const ViewProjection& viewprojection) {
	if (Draw_) {
		model_->Draw(worldTransform_, viewprojection, false);
	}
	
	if (skill_) {
		for (int i = 0; i < 3; ++i) {
			if (shockData_[i].isActive) {
				shockwave_[i]->Draw(worldTransformShockwave_[i], viewprojection, false);
			}
			
		}
	}
	
	
	RenderCollisionBounds(worldTransform_, viewprojection);
};

void RenjuBullet::OnCollision(Collider* collider) {
	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		Draw_ = false;
	}
}

const Vector3 RenjuBullet::GetWorldPosition() const {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
