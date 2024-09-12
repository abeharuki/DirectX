#include "GameScene.h"
#include "Framework/SceneManager.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>
#include <numbers>
#include <PostEffects/PostEffect.h>

void GameScene::Initialize() {

	viewProjection_.Initialize();
	viewProjection_.translation_ = { 2.2f, 3.5f, -15.0f };

	backGroundSprite_[0].reset(Sprite::Create("resources/Stage/backGround_1.png"));
	backGroundSprite_[1].reset(Sprite::Create("resources/Stage/backGround_2.png"));
	backGroundSprite_[2].reset(Sprite::Create("resources/Stage/backGround_3.png"));
	backGroundSprite_[3].reset(Sprite::Create("resources/Stage/backGround_4.png"));
	backGroundSprite_[4].reset(Sprite::Create("resources/Stage/floor.png"));
	for (int i = 0; i < 5; ++i) {
		backGroundSprite_[i]->SetSize({ 1024,704 });
	}

	debugPlayer_ = std::make_unique<DebugPlayer>();
	debugPlayer_->Initialize();

	debugEnemy_ = std::make_unique<DebugEnemy>();
	debugEnemy_->Initialize();

	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	debugPlayer_->SetStage(stage_.get());
	particleCount_ = 30;
	emitter_ = {
		.translate = {0,2.8f,0},
		.count{50},
		.frequency{0.075f},
		.frequencyTime{0.1f},
		.scaleRange{.min{0.1f,0.1f,0.0f},.max{0.2f,0.2f,0.0f}},
		.translateRange{.min{-0.25f,0.f,0.0f},.max{0.25f,0.1f,0.0f}},
		.colorRange{.min{1.f,0.3f,0.f},.max{1.f,0.4f,0.0f}},
		.lifeTimeRange{.min{0.1f},.max{0.15f}},
		.velocityRange{.min{-0.2f,-0.1f,0.0f},.max{0.2f,-0.5f,0.3f}},
	};

	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));
	
	PostEffect::GetInstance()->isGrayscale(false);

	vignetting_.intensity = 16.0f;
	smoothing_.kernelSize = 2.0f;
	outLine_.isEnable = true;
	outLine_.differenceValue = 0.1f;
	radialBlur_.center = { 0.5f,0.5f };
	radialBlur_.blurWidth = 0.01f;
	dissolve_.edgeColor = { 1.0f,-1.0f,-1.0f };
	bloom_.stepWidth = 0.001f;
	bloom_.sigma = 0.005f;
	postEffects[8] = true;
	
}

void GameScene::Update() {


	if (Input::PushKey(DIK_P)) {
		SceneManager::GetInstance()->ChangeScene("ClearScene");
	}



	emitter_.count = particleCount_;
	particle_->SetEmitter(emitter_);
	if (particleFlag_) {
		particle_->Update();
	}
	

	if (shakeFlag_) {
		particle_->Update();
		shake_.x = RandomGenerator::GetRandomFloat(-5.0f, 5.0f);
		shake_.y = RandomGenerator::GetRandomFloat(-5.0f, 5.0f);
		backGroundPos_.x += shake_.x;
		backGroundPos_.y += shake_.y;
		shakeFlag_ = false;
	}
	else {
		shake_ = { 0.0f,0.0f };
		backGroundPos_ = { 0.0f,0.0f };
		
		if (debugPlayer_->GetHitBlock()) {
			shakeFlag_ = true;
		}
	}
	for (int i = 0; i < 5; ++i) {
		backGroundSprite_[i]->SetPosition(backGroundPos_);
	}

	stage_->SetShake(shake_);
	stage_->SetDebugEnemy(debugEnemy_.get());
	stage_->Update();

	debugPlayer_->Update();
	emitter_.translate.x = debugPlayer_->GetTransformTranslate().x;
	debugEnemy_->SetDebugPlayer(debugPlayer_.get());
	debugEnemy_->Update();


	viewProjection_.UpdateMatrix();



	grayscale_.isEnable = postEffects[0];
	vignetting_.isEnable = postEffects[1];
	smoothing_.isEnable = postEffects[2];
	outLine_.isEnable = postEffects[3];
	radialBlur_.isEnble = postEffects[4];
	dissolve_.isEnble = postEffects[5];
	random_.isEnble = postEffects[6];

	isBlur_ = postEffects[7];
	bloom_.isEnble = postEffects[8];
	PostEffect* const posteffect = PostEffect::GetInstance();

	posteffect->isGrayscale(grayscale_.isEnable);
	posteffect->Vignette(vignetting_);
	posteffect->isGaussian(smoothing_.isEnable);
	posteffect->GaussianKernelSize(smoothing_.kernelSize);
	posteffect->isOutLine(outLine_.isEnable);
	posteffect->ValueOutLine(outLine_.differenceValue);
	posteffect->isRadialBlur(radialBlur_.isEnble);
	posteffect->RadialBlurCenter(radialBlur_.center);
	posteffect->RadialBlurWidth(radialBlur_.blurWidth);
	posteffect->isDissolve(dissolve_.isEnble);
	posteffect->dissolveThreshold(dissolve_.threshold);
	posteffect->EdgeColor(dissolve_.edgeColor);
	posteffect->isRandom(random_.isEnble);
	posteffect->SetBlurWidth(gasianBlur_.x);
	posteffect->SetBlurSigma(gasianBlur_.y);
	posteffect->isGasianBlur(isBlur_);
	/*posteffect->SetBloomSigma(bloom_.sigma);
	posteffect->SetBloomWidth(bloom_.stepWidth);
	posteffect->BloomLightStrength(bloom_.lightStrength);
	posteffect->BloomTreshold(bloom_.bloomThreshold);
	posteffect->isBloom(bloom_.isEnble);*/
	posteffect->SetHsv(hsv_);
	CheckAllCollision();

	particle_->DebugParameter();
	ImGui::Begin("Setting");
	ImGui::DragFloat3("camera", &viewProjection_.translation_.x);
	ImGui::Checkbox("ShakeFlag", &shakeFlag_);
	if (ImGui::TreeNode("Particle")) {
		ImGui::Checkbox("ParticleFlag", &particleFlag_);
		ImGui::SliderInt("ParticelCount", &particleCount_, 1, 50);
		ImGui::SliderFloat("Frequency", &emitter_.frequency, 0.0f, 5.0f);
		ImGui::DragFloat3("Pos", &emitter_.translate.x, 0.1f);
		ImGui::DragFloat3("ScaleRangeMin", &emitter_.scaleRange.min.x, 0.1f);
		ImGui::DragFloat3("ScaleRangeMax", &emitter_.scaleRange.max.x, 0.1f);
		ImGui::DragFloat3("PosRangeMin", &emitter_.translateRange.min.x, 0.1f);
		ImGui::DragFloat3("PosRangeMax", &emitter_.translateRange.max.x, 0.1f);
		ImGui::SliderFloat3("ColorMin", &emitter_.colorRange.min.x, 0.0f, 1.0f);
		ImGui::SliderFloat3("ColorMax", &emitter_.colorRange.max.x, 0.0f, 1.0f);
		ImGui::SliderFloat("lifeTimeMin", &emitter_.lifeTimeRange.min, 0.0f, 1.0f);
		ImGui::SliderFloat("lifeTimeMax", &emitter_.lifeTimeRange.max, 0.0f, 1.0f);
		ImGui::DragFloat3("VelocityMin", &emitter_.velocityRange.min.x, 0.1f);
		ImGui::DragFloat3("VelocityMax", &emitter_.velocityRange.max.x, 0.1f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Light")) {
		ImGui::DragFloat3("Pos", &directionLight_.direction.x, 0.1f);
		ImGui::DragFloat3("Color", &directionLight_.color.x, 0.1f);
		ImGui::DragFloat("Intensity", &directionLight_.intensity, 0.1f);
		ImGui::DragFloat("Env", &env_, 0.01f, 0.0f, 1.0f);
		ImGui::TreePop();

	}



	//Posteffect
	if (ImGui::TreeNode("PostEffect")) {
		// Grayscale
		//ImGui::Combo("##combo", &postCurrentItem, postItems, IM_ARRAYSIZE(postItems));
		if (ImGui::TreeNode("Grayscale")) {
			ImGui::Checkbox("isGaryscale%d", &postEffects[0]);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Vignetting")) {
			// vignetting
			ImGui::Checkbox("isVignetting", &postEffects[1]);
			ImGui::DragFloat3("VignettingColor", &vignetting_.color.x, 0.1f);
			ImGui::DragFloat("intensity", &vignetting_.intensity, 0.1f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Smoothing")) {
			ImGui::Checkbox("isSmoothing", &postEffects[2]);
			ImGui::SliderFloat("KerbelSize", &smoothing_.kernelSize, 1, 5);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("OutLine")) {
			ImGui::Checkbox("isOutLine", &postEffects[3]);
			ImGui::DragFloat("OutLine", &outLine_.differenceValue);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("RadialBlur")) {
			ImGui::Checkbox("isRadialBlur", &postEffects[4]);
			ImGui::DragFloat2("Center", &radialBlur_.center.x);
			ImGui::SliderFloat("Width", &radialBlur_.blurWidth, 0.01f, 1.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Dissolve")) {
			ImGui::Checkbox("isDissolve", &postEffects[5]);
			ImGui::SliderFloat("Thresholed", &dissolve_.threshold, 0.01f, 1.0f);
			ImGui::DragFloat3("EdgeColor", &dissolve_.edgeColor.x, 0.0f, 1.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Random")) {
			ImGui::Checkbox("Random", &postEffects[6]);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Blur")) {
			ImGui::Checkbox("Blur", &postEffects[7]);
			ImGui::DragFloat2("wight , sigma", &gasianBlur_.x, 0.001f, 0.001f, 0.05f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Bloom")) {
			ImGui::Checkbox("Bloom", &postEffects[8]);
			ImGui::DragFloat("wight", &bloom_.stepWidth, 0.001f, 0.001f, 0.05f);
			ImGui::DragFloat("sigma", &bloom_.sigma, 0.001f, 0.001f, 0.05f);
			ImGui::DragFloat("light", &bloom_.lightStrength, 0.1f, 1.0f, 5.0f);
			ImGui::DragFloat("threshold", &bloom_.bloomThreshold, 0.1f, 0.0, 1.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("HSV")) {
			ImGui::SliderFloat("Hue", &hsv_.hue, -1.0f, 1.0f);
			ImGui::SliderFloat("Saturation", &hsv_.saturation, -1.0f, 1.0f);
			ImGui::SliderFloat("Value", &hsv_.value, -1.0f, 1.0f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	ImGui::End();
}

void GameScene::Draw() {

	for (int i = 0; i < 5; ++i) {
		backGroundSprite_[i]->Draw();
	}

	stage_->Draw(viewProjection_, true);

	debugPlayer_->Draw(viewProjection_);
	debugEnemy_->Draw(viewProjection_);

	
	particle_->Draw(viewProjection_);
}

void GameScene::RenderDirect() {

}



Vector3 GameScene::GetLocalPosition()
{
	return Vector3();
}

void GameScene::CheckAllCollision() {
	//debugEnemy_->SetHitBody(false);
	
	for (int i = 0; i < Stage::kSize_; ++i) {
		//押し出し処理のためにBarr情報の受け取り
		debugEnemy_->SetBarr(Vector2{ stage_->GetPlayerAttackPos(i).x,stage_->GetPlayerAttackPos(i).y }, Vector2{ stage_->GetPlayerAttackScale(i).x, stage_->GetPlayerAttackScale(i).y },i);


		//
		if (debugEnemy_->GetAttack()) {
			if (Math::IsBoxCollision(debugEnemy_->GetPos().x - 25, debugEnemy_->GetPos().y, debugEnemy_->GetScale().x / 2.0f, debugEnemy_->GetScale().y,
				stage_->GetEnemyAttackPos(i).x, stage_->GetEnemyAttackPos(i).y, stage_->GetEnemyAttackScale(i).x, stage_->GetEnemyAttackScale(i).y)) {
				if (stage_->GetEnemyAttackScale(i).y == 64.0f) {
					stage_->SetDown(true, i);
				}
			}
		}

		//体の当たり判定
		if (Math::IsBoxCollision(debugEnemy_->GetPos().x-42, debugEnemy_->GetPos().y-30, debugEnemy_->GetScale().x*0.8f, debugEnemy_->GetScale().y,
			stage_->GetPlayerAttackPos(i).x, stage_->GetPlayerAttackPos(i).y, stage_->GetPlayerAttackScale(i).x, stage_->GetPlayerAttackScale(i).y)) {
			debugEnemy_->SetHitBody(true);
			ImGui::Begin("EnemyHitBody");
			ImGui::End();
		}

	
	}


}

