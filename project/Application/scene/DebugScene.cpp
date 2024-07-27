#include "DebugScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>
#include <numbers>

void DebugScene::Initialize() {
	//衝突マネージャーの作成
	collisionManager_ = std::make_unique<CollisionManager>();

	viewProjection_.Initialize();
	viewProjection_.rotation_.x = 0.28f;
	viewProjection_.translation_ = { 0.0f, 3.0f, -9.0f };
	worldTransform_.Initialize();
	worldTransform_.translate.z = -5.0f;
	worldTransformSkybox_.Initialize();
	worldTransformAnimation_.Initialize();
	worldTransformAnimation_.translate = { 2.0f,3.0f,-10.0f };
	worldTransformSphere_.Initialize();
	worldTransformSphere_.translate.y = 1.6f;
	worldTransformSphere_.rotate.y = -1.55f;
	worldTransformGround_.Initialize();
	worldTransformGround_.rotate.y = 1.58f;
	worldTransformLineBox_.Initialize();
	linebox_ = std::make_unique<LineBox>();
	AABB aabb = { {-1,-1,-1},{1,1,1} };
	linebox_.reset(LineBox::Create(aabb));

	//skybox
	skybox_ = std::make_unique<Skybox>();
	skybox_.reset(Skybox::Create("resources/skydome/skyCube.dds"));
	// 地面
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(Model::CreateModelFromObj("resources/JsonFile/wall.obj", "resources/JsonFile/wall.png"));

	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/monsterBall.png"));


	modelGround_.reset(Model::CreateModelFromObj("resources/Enemy/Alien.obj", "resources/Enemy/Atlas_Monsters.png"));

	loader_.reset(ModelLoader::Create("resources/JsonFile/loader.json"));

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("resources/human", "white.png", "walk.gltf"));

	debugPlayer_ = std::make_unique<DebugPlayer>();
	debugPlayer_->Initialize();


	
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

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&worldTransform_);

	// 自キャラの生成と初期化処理
	debugPlayer_->SetViewProjection(&followCamera_->GetViewProjection());

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
}

void DebugScene::Update() {
	worldTransformAnimation_.rotate.y += 0.01f;
	animation_->DirectionalLightDraw(directionLight_);
	sphere_->DirectionalLightDraw(directionLight_);
	modelGround_->DirectionalLightDraw(directionLight_);
	animation_->Update(0);
	//animation_->Environment(env_, true);

	emitter_.count = particleCount_;
	particle_->SetEmitter(emitter_);
	if (particleFlag_) {
		particle_->Update();
	}

	loader_->Update();
	debugPlayer_->Update();
	//CameraMove();
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();
	//viewProjection_ = loader_->GetCamera();
	debugPlayer_->SetViewProjection(&viewProjection_);

	worldTransformSkybox_.UpdateMatrix();
	worldTransformAnimation_.UpdateMatrix();
	worldTransformGround_.UpdateMatrix();
	worldTransformSphere_.UpdateMatrix();
	worldTransformLineBox_.UpdateMatrix();

	//loader_->GetModel("box")->Environment(env_, true);
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
	//animation_->isDissolve(animeDissolve_.isEnble);
	animation_->SetThreshold(animeDissolve_.threshold);
	//sphere_->isDissolve(animeDissolve_.isEnble);
	sphere_->SetThreshold(animeDissolve_.threshold);
	//modelGround_->isDissolve(animeDissolve_.isEnble);
	modelGround_->SetThreshold(animeDissolve_.threshold);
	animation_->SetEdgeColor(dissolve_.edgeColor);
	sphere_->SetEdgeColor(dissolve_.edgeColor);
	modelGround_->SetEdgeColor(dissolve_.edgeColor);
	PostEffect::GetInstance()->isGrayscale(grayscale_.isEnable);
	PostEffect::GetInstance()->Vignette(vignetting_);
	PostEffect::GetInstance()->isGaussian(smoothing_.isEnable);
	PostEffect::GetInstance()->GaussianKernelSize(smoothing_.kernelSize);
	PostEffect::GetInstance()->isOutLine(outLine_.isEnable);
	PostEffect::GetInstance()->ValueOutLine(outLine_.differenceValue);
	PostEffect::GetInstance()->isRadialBlur(radialBlur_.isEnble);
	PostEffect::GetInstance()->RadialBlurCenter(radialBlur_.center);
	PostEffect::GetInstance()->RadialBlurWidth(radialBlur_.blurWidth);
	PostEffect::GetInstance()->isDissolve(dissolve_.isEnble);
	PostEffect::GetInstance()->dissolveThreshold(dissolve_.threshold);
	PostEffect::GetInstance()->EdgeColor(dissolve_.edgeColor);
	PostEffect::GetInstance()->isRandom(random_.isEnble);
	PostEffect::GetInstance()->SetBlurWidth(gasianBlur_.x);
	PostEffect::GetInstance()->SetBlurSigma(gasianBlur_.y);
	PostEffect::GetInstance()->isGasianBlur(isBlur_);
	PostEffect::GetInstance()->SetBloomSigma(bloom_.sigma);
	PostEffect::GetInstance()->SetBloomWidth(bloom_.stepWidth);
	PostEffect::GetInstance()->BloomLightStrength(bloom_.lightStrength);
	PostEffect::GetInstance()->BloomTreshold(bloom_.bloomThreshold);
	PostEffect::GetInstance()->isBloom(bloom_.isEnble);
	PostEffect::GetInstance()->SetHsv(hsv_);
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
		ImGui::SliderFloat3("ColorMin", &emitter_.colorRange.min.x, 0.0f,1.0f);
		ImGui::SliderFloat3("ColorMax", &emitter_.colorRange.max.x, 0.0f,1.0f);
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

	if (ImGui::TreeNode("LineBox")) {

		ImGui::DragFloat3("Pos", &worldTransformLineBox_.translate.x, 0.1f);
		ImGui::DragFloat3("Rotate", &worldTransformLineBox_.rotate.x, 0.01f);
		ImGui::DragFloat3("Size", &worldTransformLineBox_.scale.x, 0.1f);

		ImGui::TreePop();
	}

	//ローダーオブジェクト
	if (ImGui::TreeNode("LoaderObj")) {
		ImGui::DragFloat("Env", &env_, 0.01f, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Sphere")) {

		ImGui::DragFloat3("Pos", &worldTransformSphere_.translate.x, 0.1f);
		ImGui::DragFloat3("Rotate", &worldTransformSphere_.rotate.x, 0.01f);
		ImGui::DragFloat3("Size", &worldTransformSphere_.scale.x, 0.1f);

		ImGui::TreePop();
	}

	//アニメーション
	if (ImGui::TreeNode("Animation")) {

		ImGui::DragFloat3("AnimationPos", &worldTransformAnimation_.translate.x, 0.1f);
		ImGui::DragFloat3("AnimationRotate", &worldTransformAnimation_.rotate.x, 0.01f);
		ImGui::DragFloat3("AnimationSize", &worldTransformAnimation_.scale.x, 0.1f);
		ImGui::DragFloat("Env", &env_, 0.01f, 0.0f, 1.0f);
		ImGui::SliderFloat("Thresholed", &animeDissolve_.threshold, 0.01f, 1.0f);
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
	
	loader_->Draw(viewProjection_, true);
	//animation_->Draw(worldTransformAnimation_, viewProjection_, true);
	//modelGround_->Draw(worldTransformGround_, viewProjection_, true);
	//debugPlayer_->Draw(viewProjection_);

	//sphere_->Draw(worldTransformSphere_, viewProjection_, true);
	//ground_->Draw(viewProjection_, false);
	//skybox_->Draw(worldTransform_, viewProjection_);
	particle_->Draw(viewProjection_);
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
		const float value = 0.7f;
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
	collisionManager_->SetColliderList(debugPlayer_.get());
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
