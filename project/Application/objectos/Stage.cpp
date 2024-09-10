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
		returnScale_[i] = false;
		AABB aabb = { {-1.0f,-1.0f,-1.0f },{1.f,1.f,1.f} };
		colliderManager_[i] = std::make_unique<ColliderManager>();
		colliderManager_[i]->SetAABB(aabb);
		colliderManager_[i]->SetCollisionMask(kCollisionMaskWall);
		colliderManager_[i]->SetCollisionAttribute(kCollisionAttributeLoderWall);
		colliderManager_[i]->SetCollisionPrimitive(kCollisionPrimitiveAABB);
		colliderManager_[i]->SetWorldTransform(worldTransform_[i]);
		colliderManager_[i]->SetEnemyHit(false);
		hitPlayer_[i] = false;
	}
	sprite_.reset(Sprite::Create("resources/Stage/background.png"));
	
}

void Stage::Update(){
	
	
	

	for (int i = 0; i < 21; ++i) {
		

		if (!returnScale_[i]) {
			//伸びる
			if (worldTransform_[i].scale.y < 3.1f) {
				worldTransform_[i].scale.y += 0.5f;
				if (hitPlayer_[i]) {
					worldTransform_[i].translate.y += 0.5f;
				}
				else {
					worldTransform_[i].translate.y -= 0.5f;
				}
				
			}
			else {
				if (hitPlayer_[i]) {
					worldTransform_[i].translate.y = 6.0f;
				}
				else {
					worldTransform_[i].translate.y = 1.0f;
				}
				
				worldTransform_[i].scale.y = 3.0f;
				returnScale_[i] = true;
				
			}
		}
		else {
			//元に戻る
			if (worldTransform_[i].scale.y > 0.5f) {
				worldTransform_[i].scale.y -= 0.05f;
				if (hitPlayer_[i]) {
					worldTransform_[i].translate.y -= 0.05f;
				}
				else {
					worldTransform_[i].translate.y += 0.05f;
				}
				
			}
			else {
				if (hitPlayer_[i]) {
					hitPlayer_[i] = false;
					upBox_[i] = false;
				}
				else {
					downBox_[i] = false;
				}
				worldTransform_[i].translate.y = 3.5f;
				worldTransform_[i].scale.y = 0.5f;
			}
		}
		
		worldTransform_[i].UpdateMatrix();
		colliderManager_[i]->SetWorldTransform(worldTransform_[i]);

		if (colliderManager_[i]->GetPlayerHit()) {
			if (worldTransform_[i].scale.y == 0.5f) {
				returnScale_[i] = false;
				hitPlayer_[i] = true;
				upBox_[i] = true;
			}
			colliderManager_[i]->SetPlayerHit(false);
		}

		if (colliderManager_[i]->GetEnemyHit() ){
			if (debugEnemy_->GetAttack() && worldTransform_[i].scale.y == 0.5f) {
				returnScale_[i] = false;
				downBox_[i] = true;
			}
			colliderManager_[i]->SetEnemyHit(false);
		}
		
	}
	


	ImGui::Begin("Stage");
	ImGui::DragFloat3("Pos", &worldTransform_[0].translate.x, 0.1f);
	ImGui::DragFloat3("Scale", &worldTransform_[0].scale.x, 0.1f);
	ImGui::Checkbox("box", &returnScale_[0]);
	ImGui::End();
}

void Stage::Draw(ViewProjection viewprojection, bool light){
	for (int i = 0; i < 21; ++i) {
		//model_[i]->Draw(worldTransform_[i], viewprojection, true);
		colliderManager_[i]->Draw(viewprojection);
	}
	//sprite_->Draw();
}


void Stage::OnCollision(Collider* collider) {
	colliderManager_->get()->OnCollision(collider);
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

