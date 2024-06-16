#include "DebugScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>
#include <numbers>

void DebugScene::Initialize() {
	viewProjection_.Initialize();
	viewProjection_.rotation_.x = 0.28f;
	viewProjection_.translation_ = { 0.0f, 3.0f, -9.0f };
	worldTransform_.Initialize();
	worldTransform_.translate.z = -5.0f;
	worldTransformSkybox_.Initialize();
	worldTransformAnimation_.Initialize();
	worldTransformAnimation_.translate = { 2.3f,2.8f,-15.5f };
	//skybox
	skybox_ = std::make_unique<Skybox>();
	skybox_.reset(Skybox::Create("resources/skydome/skyCube.dds"));
	// 地面
	ground_ = std::make_unique<Ground>();
	ground_->Initialize(
		Model::CreateModelFromObj("resources/ground/ground.obj", "resources/ground/ground.png"));


	loader_.reset(ModelLoader::Create("resources/JsonFile/loader.json"));

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("resources/human", "white.png", "walk.gltf"));

	//modelBunny_.reset(Model::CreateModelFromObj("resources/bunny.obj", "resources/moon.png"));

	debugPlayer_ = std::make_unique<DebugPlayer>();
	debugPlayer_->Initialize();

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&debugPlayer_->GetWorldTransform());

	// 自キャラの生成と初期化処理
	debugPlayer_->SetViewProjection(&followCamera_->GetViewProjection());

	PostEffect::GetInstance()->isGrayscale(false);

	vignetting_.intensity = 16.0f;
	smoothing_.kernelSize = 2.0f;
	outLine_.differenceValue = 0.1f;
}

void DebugScene::Update() {


	//animation_->Update(0);
	//animation_->Environment(env_, true);

	loader_->Update();
	debugPlayer_->Update();
	//CameraMove();
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();
	
	worldTransformSkybox_.UpdateMatrixQuaternion();
	worldTransformAnimation_.UpdateMatrixQuaternion();


	ground_->Update();
	loader_->GetModel("box")->Environment(env_, true);
	grayscale_.isEnable = postEffects[0];
	vignetting_.isEnable = postEffects[1];
	smoothing_.isEnable = postEffects[2];
	outLine_.isEnable = postEffects[3];
	if (outLine_.isEnable != 0) {
		PostEffect::GetInstance()->Effect(false);
	}
	else
	PostEffect::GetInstance()->isGrayscale(grayscale_.isEnable);
	PostEffect::GetInstance()->Vignette(vignetting_);
	PostEffect::GetInstance()->isGaussian(smoothing_.isEnable);
	PostEffect::GetInstance()->GaussianKernelSize(smoothing_.kernelSize);
	PostEffect::GetInstance()->isOutLine(outLine_.isEnable,viewProjection_);
	PostEffect::GetInstance()->ValueOutLine(outLine_.differenceValue);
	ImGui::Begin("Setting");
	/*
	//ローダーオブジェクト
	if (ImGui::TreeNode("LoaderObj")) {
		ImGui::DragFloat("Env", &env_, 0.01f, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	
	
	//アニメーション
	if (ImGui::TreeNode("Animation")) {

		ImGui::DragFloat3("AnimationPos", &worldTransformAnimation_.translate.x, 0.1f);
		ImGui::DragFloat3("AnimationRotate", &worldTransformAnimation_.rotate.x, 0.01f);
		ImGui::DragFloat3("AnimationSize", &worldTransformAnimation_.scale.x, 0.1f);
		ImGui::DragFloat("Env", &env_, 0.01f, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	*/
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

		ImGui::TreePop();
	}

	ImGui::End();
}

void DebugScene::Draw() {
	//loader_->Draw(viewProjection_, true);
	//animation_->Draw(worldTransformAnimation_, viewProjection_, true);
	debugPlayer_->Draw(viewProjection_);
	ground_->Draw(viewProjection_, false);
	skybox_->Draw(worldTransform_, viewProjection_);
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

Vector3 DebugScene::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.translate.x;
	worldPos.y = worldTransform_.translate.y;
	worldPos.z = worldTransform_.translate.z;
	return worldPos;
}
