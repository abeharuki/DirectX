#include "Stage.h"
#include <CollisionManager/CollisionConfig.h>

void Stage::Initialize(){
	

	for (int i = 0; i < kSize_; ++i) {
		playerAttackSprite_[i].reset(Sprite::Create("resources/Stage/pillar_player.png"));
		enemyAttackSprite_[i].reset(Sprite::Create("resources/Stage/pillar_boss.png"));
		boxSprite_[i].reset(Sprite::Create("resources/Stage/block.png"));
		boxSprite_[i]->SetSize({ 64.0f,64.0f });
		boxPos_[i] = Vector2{64.0f * i ,331.f};
		
		
		playerAttackTransform_[i].translate.y = 331.0f;
		playerAttackTransform_[i].translate.x += 64.0f * i;
		playerAttackTransform_[i].scale = { 64.0f,64.0f,0.0f };
		enemyAttackTransform_[i].translate.y = 331.0f;
		enemyAttackTransform_[i].translate.x = 64.0f*i;
		enemyAttackTransform_[i].scale = { 64.0f,64.0f,0.0f };
		
		returnScale_[i] = false;
		
		hitPlayer_[i] = false;
	}
	
	
}

void Stage::Update(){
	

	

	for (int i = 0; i < kSize_; ++i) {
		
		

		if (!returnScale_[i]) {
			//伸びる
			if (upBox_[i]) {
				if (playerAttackTransform_[i].scale.y < 394.0f) {
					playerAttackTransform_[i].scale.y += 50.f;
					playerAttackTransform_[i].translate.y -= 50;
				}
				else {
					playerAttackTransform_[i].scale.y = 394.0f;
					playerAttackTransform_[i].translate.y = 1.0f;
					returnScale_[i] = true;
				}
			}

			if (downBox_[i]) {
				if (enemyAttackTransform_[i].scale.y < 321.0f) {
					enemyAttackTransform_[i].scale.y += 50.f;
				}
				else {
					enemyAttackTransform_[i].scale.y = 321.0f;
					returnScale_[i] = true;
				}
			}

		}
		else {
			//元に戻る
			if (upBox_[i]) {
				if (playerAttackTransform_[i].scale.y > 64.0f) {
					playerAttackTransform_[i].translate.y += 7.f;
					playerAttackTransform_[i].scale.y -= 7.f;
					
				}
				else {
					playerAttackTransform_[i].scale.y = 64.0f;
					playerAttackTransform_[i].translate.y = 331.0f;
					upBox_[i] = false;
					returnScale_[i] = false;
				}
			}

			if (downBox_[i]) {
				if (enemyAttackTransform_[i].scale.y > 64.0f) {
					enemyAttackTransform_[i].scale.y -= 7.f;
				}
				else {
					enemyAttackTransform_[i].scale.y = 64.0f;
					enemyAttackTransform_[i].translate.y = 331.0f;
					downBox_[i] = false;
					returnScale_[i] = false;
				}
			}

		}
		
		if (shake_.x != 0 || shake_.y != 0) {
			boxPos_[i].x += shake_.x;
			boxPos_[i].y += shake_.y;
			playerAttackTransform_[i].translate += Vector3{ shake_.x,shake_.y,0.0f };
			enemyAttackTransform_[i].translate += Vector3{ shake_.x,shake_.y,0.0f };
		}
		else {
			boxPos_[i] = Vector2{ 64.0f * i ,331.f };
			if (!upBox_[i]) {
				playerAttackTransform_[i].translate.y = 331.0f;

			}

			playerAttackTransform_[i].translate.x += 64.0f * i;
			enemyAttackTransform_[i].translate.x = 64.0f * i;
			enemyAttackTransform_[i].translate.y = 331.0f;
		}

		boxSprite_[i]->SetPosition(boxPos_[i]);
		playerAttackSprite_[i]->SetTransform(playerAttackTransform_[i]);
		enemyAttackSprite_[i]->SetTransform(enemyAttackTransform_[i]);
	}
	


	ImGui::Begin("Stage");
	ImGui::DragFloat3("Pos", &enemyAttackTransform_[0].translate.x, 0.1f);
	ImGui::DragFloat3("Scale", &enemyAttackTransform_[0].scale.x, 0.1f);
	ImGui::Checkbox("box", &returnScale_[0]);
	ImGui::DragInt("BoxNum", &boxNum_,1,0,16);
	ImGui::Checkbox("up", &upBox_[boxNum_]);
	ImGui::Checkbox("down", &downBox_[boxNum_]);
	ImGui::End();
}

void Stage::Draw(ViewProjection viewprojection, bool light){
	//backGround_->Draw();
	for (int i = 0; i < kSize_; ++i) {
		playerAttackSprite_[i]->Draw();
		enemyAttackSprite_[i]->Draw();
		boxSprite_[i]->Draw();
	}
}

