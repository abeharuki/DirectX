#include "Stage.h"
#include <CollisionManager/CollisionConfig.h>

void Stage::Initialize(){
	

	for (int i = 0; i < 21; ++i) {
		model_[i].reset(Model::CreateModelFromObj("resources/box.obj", "resources/Enemy/white.png"));
		worldTransform_[i].Initialize();
		worldTransform_[i].translate.x = -10.0f;
		worldTransform_[i].translate.y = 3.5f;
		worldTransform_[i].translate.x += i * 1.f;
		worldTransform_[i].scale = { 0.5f,0.5f,0.5f };
		AABB aabb = { {-0.5f,-0.5f,-0.5f },{0.5f,0.5f,0.5f} };
		OBB obb = Math::ConvertAABBToOBB(aabb);
		colliderManager_[i] = std::make_unique<ColliderManager>();
		colliderManager_[i]->SetOBB(obb);
		colliderManager_[i]->SetCollisionMask(kCollisionMaskWall);
		colliderManager_[i]->SetCollisionAttribute(kCollisionAttributeLoderWall);
		colliderManager_[i]->SetCollisionPrimitive(kCollisionPrimitiveOBB);
		colliderManager_[i]->SetWorldTransform(worldTransform_[i]);
	}

}

void Stage::Update(){
	for (int i = 0; i < 21; ++i) {
		worldTransform_[i].UpdateMatrix();
	}
	


	ImGui::Begin("Stage");
	ImGui::DragFloat3("Pos", &worldTransform_[0].translate.x, 0.1f);
	ImGui::End();
}

void Stage::Draw(ViewProjection viewprojection, bool light){
	for (int i = 0; i < 21; ++i) {
		model_[i]->Draw(worldTransform_[i], viewprojection, true);
	}
	
}


void Stage::OnCollision(Collider* collider) {

	
}

const Vector3 Stage::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

