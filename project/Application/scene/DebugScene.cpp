#include "DebugScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>
#include <numbers>



void DebugScene::Initialize() {
	//衝突マネージャーの作成
	collisionManager_ = std::make_unique<CollisionManager>();

	colliderManager_[0] = std::make_unique<ColliderManager>();
	colliderManager_[1] = std::make_unique<ColliderManager>();
	colliderManager_[2] = std::make_unique<ColliderManager>();
	AABB aabb = { {-7.0f,-0.2f,-1 },{7.0f,0.2f,1} };
	AABB aabb2 = { {-1,-1,-1 },{1,1,1} };
	OBB obb = Math::ConvertAABBToOBB(aabb);

	colliderManager_[0]->SetOBB(obb);
	colliderManager_[0]->SetCollisionMask(kCollisionMaskEnemy);
	colliderManager_[0]->SetCollisionAttribute(kCollisionAttributeEnemy);
	colliderManager_[0]->SetCollisionPrimitive(kCollisionPrimitiveOBB);



	colliderManager_[1] = std::make_unique<ColliderManager>();
	colliderManager_[1]->SetAABB(aabb2);
	colliderManager_[1]->SetCollisionMask(kCollisionMaskPlayer);
	colliderManager_[1]->SetCollisionAttribute(kCollisionAttributePlayer);
	colliderManager_[1]->SetCollisionPrimitive(kCollisionPrimitiveOBB);


	colliderManager_[2] = std::make_unique<ColliderManager>();
	colliderManager_[2]->SetRadius(1.0f);
	colliderManager_[2]->SetCollisionMask(kCollisionMaskEnemy);
	colliderManager_[2]->SetCollisionAttribute(kCollisionAttributeEnemy);
	colliderManager_[2]->SetCollisionPrimitive(kCollisionPrimitiveSphere);

	viewProjection_.Initialize();
	viewProjection_.rotation_.x = 0.28f;
	viewProjection_.translation_ = { 0.0f, 3.0f, -9.0f };
	worldTransform_.Initialize();
	worldTransform_.translate.z = -5.0f;
	worldTransformSkybox_.Initialize();
	worldTransformAnimation_.Initialize();
	worldTransformAnimation_.rotate.y = 3.14f;
	worldTransformAnimation_.scale = { 1.f,1.f,1.f };
	worldTransformCollider1_.Initialize();
	worldTransformCollider1_.rotate = { 0.0f,0.2f,0.0f };
	worldTransformCollider1_.translate = { 0.0f,0.0f,0.3f };

	worldTransformCollider2_.Initialize();
	worldTransformCollider3_.Initialize();
	worldTransformModel_.Initialize();
	worldTransformModel_.rotate.y = 3.14f;
	worldTransformModel_.scale = { 1.f,1.f,1.f };
	worldTransformSter_[0].Initialize();
	worldTransformSter_[1].Initialize();
	sprite_.reset(Sprite::Create("resources/DDS/cat.dds"));


	skybox_.reset(Skybox::Create("resources/skydome/skyCube.dds"));

	model_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/Enemy/num/0.png"));
	model_2.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));
	ster_[0].reset(Model::CreateModelFromObj("resources/Enemy/ster.obj", "resources/Enemy/ster.png"));
	ster_[1].reset(Model::CreateModelFromObj("resources/Enemy/ster.obj", "resources/Enemy/ster.png"));
	loader_.reset(ModelLoader::Create("resources/JsonFile/loader.json"));

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Renju", "Atlas.png", "renju.gltf"));

	emitter_ = {
		.translate = {0,3,0},
		.count{50},
		.frequency{0.075f},
		.frequencyTime{0.5f},
		.scaleRange{.min{1.0f,1.0f,11.0f},.max{1.0f,1.0f,1.0f}},
		.translateRange{.min{-3.f,-3.f,-3.f},.max{3.f,3.f,3.f}},
		.colorRange{.min{0.33f,0,0.33f},.max{0.5f,0,1.0f}},
		.alphaRange{.min{1.0f},.max{1.0f}},
		.lifeTimeRange{.min{0.1f},.max{1.0f}},
		.velocityRange{.min{0.0f,0.f,0.0f},.max{0.0f,0.f,0.f}},
		
	};

	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&worldTransform_);
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
	directionLight_.direction.z = 2;
	AnimationNum_ = 4;
	a_ = 1.0f;

	angle_[0] = 0.0f;
	angle_[1] = 2.0f;
	angle_[2] = 4.0f;

}

void DebugScene::Update() {
	animation_->Stop(animationStop_);
	
	animation_->DirectionalLightDraw(directionLight_);
	animation_->Update(AnimationNum_);
	animation_->SetFlameTimer(animaflame_);
	model_->DirectionalLightDraw(directionLight_);
	model_->SetColor({ 1.0f,1.0f,1.0f,a_ });
	emitter_.count = particleCount_;
	emitter_.isScaleChanging = scaleFlag_;
	gravityFiled_.min = emitter_.translateRange.min;
	gravityFiled_.max = emitter_.translateRange.max;
	gravityFiled_.translate = emitter_.translate;
	accelerationFiled_.min = emitter_.translateRange.min;
	accelerationFiled_.max = emitter_.translateRange.max;
	accelerationFiled_.translate = emitter_.translate;
	particle_->SetEmitter(emitter_);
	particle_->SetAccelerationFiled(accelerationFiled_);
	particle_->SetGravityFiled(gravityFiled_);
	if (particleFlag_) {
		particle_->Update();
	}

	loader_->Update();

	if (blendNum_ == 0) {
		blendMode_ = BlendMode::kNone;
	}
	else if (blendNum_ == 1) {
		blendMode_ = BlendMode::kNormal;
	}else if (blendNum_ == 2) {
		blendMode_ = BlendMode::kAdd;
	}
	else if (blendNum_ == 3) {
		blendMode_ = BlendMode::kSubtract;
	}
	else if (blendNum_ == 4) {
		blendMode_ = BlendMode::kMultily;
	}



	model_2->SetBlendMode(blendMode_);
	model_2->SetColor(modelColor_);



	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();


	worldTransformSkybox_.UpdateMatrix();
	worldTransformAnimation_.UpdateMatrix();
	worldTransformCollider1_.UpdateMatrix();
	worldTransformCollider2_.UpdateMatrix();
	worldTransformCollider3_.UpdateMatrix();
	worldTransformModel_.UpdateMatrix();
	worldTransformSter_[0].UpdateMatrix();
	worldTransformSter_[1].UpdateMatrix();
	colliderManager_[0]->SetWorldTransform(worldTransformCollider1_);
	colliderManager_[1]->SetWorldTransform(worldTransformCollider2_);
	colliderManager_[2]->SetWorldTransform(worldTransformCollider3_);



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
	posteffect->RadialBlurRotation(radialBlur_.rotation);
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

	particle_->DebugParameter();
	ImGui::Begin("Setting");
	ImGui::Text("Angle%f", angle_[0]);
	if (ImGui::TreeNode("Particle")) {
		ImGui::Checkbox("ParticleFlag", &particleFlag_);
		ImGui::SliderInt("ParticelCount", &particleCount_, 1, 50);
		ImGui::SliderFloat("Frequency", &emitter_.frequency, 0.0f, 5.0f);
		ImGui::DragFloat3("Pos", &emitter_.translate.x, 0.1f);
		ImGui::DragFloat3("PosRangeMin", &emitter_.translateRange.min.x, 0.1f);
		ImGui::DragFloat3("PosRangeMax", &emitter_.translateRange.max.x, 0.1f);
		ImGui::DragFloat3("ScaleRangeMin", &emitter_.scaleRange.min.x, 0.01f);
		ImGui::DragFloat3("ScaleRangeMax", &emitter_.scaleRange.max.x, 0.01f);
		
		ImGui::SliderFloat3("ColorMin", &emitter_.colorRange.min.x, 0.0f, 1.0f);
		ImGui::SliderFloat3("ColorMax", &emitter_.colorRange.max.x, 0.0f, 1.0f);
		ImGui::SliderFloat("AlphaMin", &emitter_.alphaRange.min, 0.0f, 10.0f);
		ImGui::SliderFloat("AlphaMax", &emitter_.alphaRange.max, 0.0f, 10.0f);
		ImGui::SliderFloat("lifeTimeMin", &emitter_.lifeTimeRange.min, 0.0f, 1.0f);
		ImGui::SliderFloat("lifeTimeMax", &emitter_.lifeTimeRange.max, 0.0f, 1.0f);
		ImGui::DragFloat3("VelocityMin", &emitter_.velocityRange.min.x, 0.1f);
		ImGui::DragFloat3("VelocityMax", &emitter_.velocityRange.max.x, 0.1f);
		ImGui::Checkbox("scaleFlag", &scaleFlag_);
		ImGui::SliderFloat("endAlpha", &emitter_.endAlpha, 0.0f, 10.0f);
		ImGui::SliderFloat3("scaleAlpha", &emitter_.endScale.x, 0.0f, 3.0f);

		ImGui::SliderFloat3("Acceleration", &accelerationFiled_.acceleration.x, 0.0f, 3.0f);
		ImGui::SliderFloat("GravityStrength", &gravityFiled_.strength, 0.0f, 10.0f);
		ImGui::SliderFloat("GravityStop", &gravityFiled_.stopDistance, 0.0f, 10.0f);
		
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
		ImGui::SliderFloat4("modelColor", &modelColor_.x, -1.0f, 1.0f);
		ImGui::DragInt("Blend", &blendNum_, 1,0,4);
		ImGui::TreePop();
	}
	//ローダーオブジェクト
	if (ImGui::TreeNode("LoaderObj")) {
		ImGui::DragFloat("Env", &env_, 0.01f, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("ColldierOBBEnemy")) {

		ImGui::DragFloat3("Pos", &worldTransformCollider1_.translate.x, 0.1f);
		ImGui::DragFloat3("Rotate", &worldTransformCollider1_.rotate.x, 0.01f);
		ImGui::DragFloat3("Size", &worldTransformCollider1_.scale.x, 0.1f);
		ImGui::DragFloat3("InitilizePos", &impactPos_.x, 0.1f);
		ImGui::DragFloat3("Velocity", &impactVelocity_.x, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("ColldierAABBPlayer")) {

		ImGui::DragFloat3("Pos", &worldTransformCollider2_.translate.x, 0.1f);
		ImGui::DragFloat3("Rotate", &worldTransformCollider2_.rotate.x, 0.01f);
		ImGui::DragFloat3("Size", &worldTransformCollider2_.scale.x, 0.1f);

		ImGui::TreePop();
	}

	//アニメーション
	if (ImGui::TreeNode("Animation")) {
		ImGui::Checkbox("StopAnimation", &animationStop_);
		ImGui::SliderInt("AnimationNum", &AnimationNum_, 0, 4);
		ImGui::SliderFloat("AnimationFlame", &animaflame_, 0, 120);
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
			ImGui::SliderFloat("Rotation", &radialBlur_.rotation, 0.01f, 1.0f);
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


	animation_->Draw(worldTransformAnimation_, viewProjection_, true);
	colliderManager_[0]->Draw(viewProjection_);
	colliderManager_[1]->Draw(viewProjection_);

	skybox_->Draw(worldTransformSkybox_, viewProjection_);
	loader_->Draw(viewProjection_, true);
	particle_->Draw(viewProjection_);
	//model_->Draw(worldTransformAnimation_, viewProjection_, false);
	model_2->Draw(worldTransformModel_, viewProjection_, false);
	/*ster_[0]->Draw(worldTransformSter_[0],viewProjection_,true);
	ster_[1]->Draw(worldTransformSter_[1], viewProjection_, true);*/
	//sprite_->Draw();

	/*debugPlayer_->Draw(viewProjection_);
	debugEnemy_->Draw(viewProjection_);

	model_->Draw(worldTransformModel_, viewProjection_, true);*/
}

void DebugScene::RenderDirect() {

}

void DebugScene::CameraMove() {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		const float value = 0.7f;
		bool isMove = false;

		// 移動速度
		const float kCharacterSpeed = 0.2f;
		// 移動量
		velocity_ = {
		   (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		   (float)joyState.Gamepad.sThumbLY / SHRT_MAX };

		if (Math::Length(velocity_) > value) {
			isMove = true;
			velocity_ = Math::Normalize(velocity_);
			velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		}



		if (isMove) {
			Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(viewProjection_.rotation_.y);
			velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
			// 現在の位置から移動する位置へのベクトル
			Vector3 sub = (worldTransform_.translate + velocity_) - GetLocalPosition();
			// 平行移動
			worldTransform_.translate = Math::Add(worldTransform_.translate, velocity_);
			if (sub.z != 0.0) {
				destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

				if (sub.z < 0.0) {
					destinationAngleY_ = (sub.x >= 0.0)
						? std::numbers::pi_v<float> -destinationAngleY_
						: -std::numbers::pi_v<float> -destinationAngleY_;
				}
			}
			else {
				destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f
					: -std::numbers::pi_v<float> / 2.0f;
			}
		}



	}
	else {
		//const float value = 0.7f;
		bool isMove_ = false;
		/*----------移動処理----------*/
		float kCharacterSpeed = 0.3f;
		// 移動量
		velocity_ = { 0.0f, 0.0f, 0.0f };

		// 上下移動
		if (Input::PressKey(DIK_W)) {
			velocity_.z = 1;

		}
		else if (Input::PressKey(DIK_S)) {
			velocity_.z = -1;
		}


		// 左右移動
		if (Input::PressKey(DIK_A)) {
			velocity_.x = -1;

		}
		else if (Input::PressKey(DIK_D)) {
			velocity_.x = 1;
		}

		if (Input::PressKey(DIK_A) || Input::PressKey(DIK_D) || Input::PressKey(DIK_W) ||
			Input::PressKey(DIK_S)) {
			isMove_ = true;
			velocity_ = Math::Normalize(velocity_);
			velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		}

		if (isMove_) {
			Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(viewProjection_.rotation_.y);
			velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
			// 現在の位置から移動する位置へのベクトル
			Vector3 sub = (worldTransform_.translate + velocity_) - GetLocalPosition();
			// 平行移動
			worldTransform_.translate = Math::Add(worldTransform_.translate, velocity_);
			if (sub.z != 0.0) {
				destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

				if (sub.z < 0.0) {
					destinationAngleY_ = (sub.x >= 0.0)
						? std::numbers::pi_v<float> -destinationAngleY_
						: -std::numbers::pi_v<float> -destinationAngleY_;
				}
			}
			else {
				destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f
					: -std::numbers::pi_v<float> / 2.0f;
			}
		}

	}

	// 回転
	worldTransform_.rotate.y = Math::LerpShortAngle(worldTransform_.rotate.y, destinationAngleY_, 0.5f);

	worldTransform_.UpdateMatrix();
}

void DebugScene::CheckAllCollision() {
	//コリジョン関係
	collisionManager_->ClearColliderList();
	collisionManager_->SetColliderList(colliderManager_[0].get());
	collisionManager_->SetColliderList(colliderManager_[1].get());
	//collisionManager_->SetColliderList(colliderManager_[2].get());
	for (int i = 0; i < loader_->GetColliderSize(); ++i) {
		collisionManager_->SetColliderList(loader_->GetCollider(i));
	}
	collisionManager_->CheckAllCollisions();
}

Vector3 DebugScene::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.translate.x;
	worldPos.y = worldTransform_.translate.y;
	worldPos.z = worldTransform_.translate.z;
	return worldPos;
}

