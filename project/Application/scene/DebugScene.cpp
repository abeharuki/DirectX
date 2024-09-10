#include "DebugScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>
#include <numbers>



void DebugScene::Initialize() {
	//衝突マネージャーの作成
	collisionManager_ = std::make_unique<CollisionManager>();

	viewProjection_.Initialize();
	viewProjection_.translation_ = { 2.2f, 3.5f, -15.0f };
	worldTransform_.Initialize();
	worldTransform_.translate.z = -5.0f;
	worldTransformSkybox_.Initialize();
	worldTransformAnimation_.Initialize();

	worldTransformModel_.Initialize();
	//worldTransformModel_.scale = { 15.0f,0.5f,0.0f };
	worldTransformModel_.translate.y = 3.5f;
	backGroundSprite_[0].reset(Sprite::Create("resources/Stage/backGround_1.png"));
	backGroundSprite_[1].reset(Sprite::Create("resources/Stage/backGround_2.png"));
	backGroundSprite_[2].reset(Sprite::Create("resources/Stage/backGround_3.png"));
	backGroundSprite_[3].reset(Sprite::Create("resources/Stage/backGround_4.png"));
	backGroundSprite_[4].reset(Sprite::Create("resources/Stage/floor.png"));


	skybox_.reset(Skybox::Create("resources/skydome/skyCube.dds"));

	model_.reset(Model::CreateModelFromObj("resources/box.obj", "resources/Enemy/red_.png"));
	loader_.reset(ModelLoader::Create("resources/JsonFile/loader.json"));

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("resources/Enemy", "Atlas_Monsters.png", "Alien2.gltf"));

	debugPlayer_ = std::make_unique<DebugPlayer>();
	debugPlayer_->Initialize();

	debugEnemy_ = std::make_unique<DebugEnemy>();
	debugEnemy_->Initialize();

	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	emitter_ = {
		.translate = {0,3,0},
		.count{50},
		.frequency{0.075f},
		.frequencyTime{0.5f},
		.scaleRange{.min{2.2f,1.0f,11.0f},.max{2.2f,3.5f,1.0f}},
		.translateRange{.min{-3.2f,-1.2f,0.0f},.max{3.2f,1.2f,0.0f}},
		.colorRange{.min{0.33f,0,0.33f},.max{0.5f,0,1.0f}},
		.lifeTimeRange{.min{0.1f},.max{1.0f}},
		.velocityRange{.min{0.0f,0.1f,0.0f},.max{0.0f,0.4f,0.3f}},
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
	AnimationNum_ = 0;
	a_ = 1.0f;
}

void DebugScene::Update() {
	animation_->DirectionalLightDraw(directionLight_);
	animation_->Update(AnimationNum_);
	animation_->SetFlameTimer(animaflame_);
	model_->DirectionalLightDraw(directionLight_);
	stage_->SetLight(directionLight_);
	emitter_.count = particleCount_;
	particle_->SetEmitter(emitter_);
	if (particleFlag_) {
		particle_->Update();
	}

	loader_->Update();
	stage_->SetDebugEnemy(debugEnemy_.get());
	stage_->Update();

	debugPlayer_->Update();
	debugEnemy_->SetDebugPlayer(debugPlayer_.get());
	debugEnemy_->Update();

	ImGui::DragFloat3("CameraPos", &viewProjection_.translation_.x);

	viewProjection_.UpdateMatrix();

	worldTransformSkybox_.UpdateMatrix();
	worldTransformAnimation_.UpdateMatrix();
	worldTransformModel_.UpdateMatrix();




	grayscale_.isEnable = postEffects[0];
	vignetting_.isEnable = postEffects[1];
	smoothing_.isEnable = postEffects[2];
	outLine_.isEnable = postEffects[3];
	radialBlur_.isEnble = postEffects[4];
	dissolve_.isEnble = postEffects[5];
	random_.isEnble = postEffects[6];
	animeDissolve_.isEnble = isAnimeDissolve_;
	isBlur_ = postEffects[7];
	bloom_.isEnble = postEffects[8];
	model_->SetThreshold(animeDissolve_.threshold);
	animation_->SetThreshold(animeDissolve_.threshold);
	animation_->SetEdgeColor(dissolve_.edgeColor);
	animation_->SetEnvironment(env_, true);

	PostEffect *const posteffect = PostEffect::GetInstance();

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
	posteffect->SetBloomSigma(bloom_.sigma);
	posteffect->SetBloomWidth(bloom_.stepWidth);
	posteffect->BloomLightStrength(bloom_.lightStrength);
	posteffect->BloomTreshold(bloom_.bloomThreshold);
	posteffect->isBloom(bloom_.isEnble);
	posteffect->SetHsv(hsv_);
	CheckAllCollision();


	ImGui::Begin("Setting");

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
	if (ImGui::TreeNode("Model")) {
		ImGui::Checkbox("scale", &impactScale_);
		ImGui::DragFloat3("Pos", &worldTransformModel_.translate.x, 0.1f);
		ImGui::DragFloat3("Rotate", &worldTransformModel_.rotate.x, 0.01f);
		ImGui::DragFloat3("Size", &worldTransformModel_.scale.x, 0.1f);
		ImGui::SliderFloat("Color", &a_, -1.0f, 1.0f);
		ImGui::TreePop();
	}
	//ローダーオブジェクト
	if (ImGui::TreeNode("LoaderObj")) {
		ImGui::DragFloat("Env", &env_, 0.01f, 0.0f, 1.0f);
		ImGui::TreePop();
	}


	//アニメーション
	if (ImGui::TreeNode("Animation")) {
		ImGui::SliderInt("AnimationNum", &AnimationNum_, 0, 7);
		ImGui::SliderFloat("AnimationFlame", &animaflame_, 0, 60);
		ImGui::DragFloat3("AnimationPos", &worldTransformAnimation_.translate.x, 0.1f);
		ImGui::DragFloat3("AnimationRotate", &worldTransformAnimation_.rotate.x, 0.01f);
		ImGui::DragFloat3("AnimationSize", &worldTransformAnimation_.scale.x, 0.1f);
		ImGui::DragFloat("Env", &env_, 0.01f, 0.0f, 1.0f);
		ImGui::SliderFloat("Thresholed", &animeDissolve_.threshold, 0.01f, 1.0f);
		ImGui::Text("AnimationTimer%f", animation_->GetAnimationTimer());
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

void DebugScene::Draw() {
	//animation_->Draw(worldTransformAnimation_, viewProjection_, true);
	//model_->Draw(worldTransformModel_, viewProjection_, true);
	/*
	colliderManager_[0]->Draw(viewProjection_);
	colliderManager_[1]->Draw(viewProjection_);

	skybox_->Draw(worldTransformSkybox_,viewProjection_);
	loader_->Draw(viewProjection_, true);
	particle_->Draw(viewProjection_);
	*/
	for (int i = 0; i < 5; ++i) {
		backGroundSprite_[i]->Draw();
	}
	debugPlayer_->Draw(viewProjection_);
	debugEnemy_->Draw(viewProjection_);

	//stage_->Draw(viewProjection_, true);
}

void DebugScene::RenderDirect() {

}



Vector3 DebugScene::GetLocalPosition()
{
	return Vector3();
}

void DebugScene::CheckAllCollision() {
	for (int i = 0; i < Stage::kSize_; ++i) {
		if (debugEnemy_->GetAttack()) {
			if (Math::IsBoxCollision(debugEnemy_->GetPos().x, debugEnemy_->GetPos().y, debugEnemy_->GetScale().x, debugEnemy_->GetScale().y,
				stage_->GetPos(i).x, stage_->GetPos(i).y, stage_->GetScale(i).x, stage_->GetScale(i).y)) {
				if (stage_->GetScale(i).y == 64.0f) {
					stage_->SetDown(true, i);
				}



			}
		}

	}


}

