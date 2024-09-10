#include "Stage.h"
#include <CollisionManager/CollisionConfig.h>

void Stage::Initialize(){
	

	for (int i = 0; i < 21; ++i) {
		boxSprite_[i].reset(Sprite::Create("resources/HPG.png"));
		spriteTransform_[i].translate.x = 1.3f;
		spriteTransform_[i].translate.y = 331.0f;
		spriteTransform_[i].translate.x = 60.0f*i;
		spriteTransform_[i].scale = { 60.0f,55.0f,0.0f };
		returnScale_[i] = false;
		
		hitPlayer_[i] = false;
	}
	backGround_.reset(Sprite::Create("resources/Stage/background.png"));
	
}

void Stage::Update(){
	

	

	for (int i = 0; i < 21; ++i) {
		

		if (!returnScale_[i]) {
			//伸びる
			if (upBox_[i]) {
				if (spriteTransform_[i].scale.y < 385.0f) {
					spriteTransform_[i].scale.y += 50.f;
					spriteTransform_[i].translate.y -= 50;
				}
				else {
					spriteTransform_[i].scale.y = 385.0f;
					spriteTransform_[i].translate.y = 1.0f;
					returnScale_[i] = true;
				}
			}

			if (downBox_[i]) {
				if (spriteTransform_[i].scale.y < 321.0f) {
					spriteTransform_[i].scale.y += 50.f;
				}
				else {
					spriteTransform_[i].scale.y = 321.0f;
					returnScale_[i] = true;
				}
			}

		}
		else {
			//元に戻る
			if (upBox_[i]) {
				if (spriteTransform_[i].scale.y > 55.0f) {
					spriteTransform_[i].translate.y += 7.f;
					spriteTransform_[i].scale.y -= 7.f;
					
				}
				else {
					spriteTransform_[i].scale.y = 55.0f;
					spriteTransform_[i].translate.y = 331.0f;
					upBox_[i] = false;
					returnScale_[i] = false;
				}
			}

			if (downBox_[i]) {
				if (spriteTransform_[i].scale.y > 55.0f) {
					spriteTransform_[i].scale.y -= 7.f;
				}
				else {
					spriteTransform_[i].scale.y = 55.0f;
					spriteTransform_[i].translate.y = 331.0f;
					downBox_[i] = false;
					returnScale_[i] = false;
				}
			}

		}
		

		/*if (colliderManager_[i]->GetPlayerHit()) {
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
		}*/
		boxSprite_[i]->SetTransform(spriteTransform_[i]);


	}
	


	ImGui::Begin("Stage");
	ImGui::DragFloat3("Pos", &spriteTransform_[0].translate.x, 0.1f);
	ImGui::DragFloat3("Scale", &spriteTransform_[0].scale.x, 0.1f);
	ImGui::Checkbox("box", &returnScale_[0]);
	ImGui::Checkbox("up", &upBox_[0]);
	ImGui::Checkbox("down", &downBox_[0]);
	ImGui::End();
}

void Stage::Draw(ViewProjection viewprojection, bool light){
	//backGround_->Draw();
	for (int i = 0; i < 21; ++i) {

		boxSprite_[i]->Draw();
	}
}

