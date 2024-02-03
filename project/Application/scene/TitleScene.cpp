#include "TitleScene.h"
#include "Framework/SceneManager.h"

void TitleScene::Initialize() { 
	audio_ = Audio::GetInstance();
	audioData_ = audio_->SoundLoadWave("resources/audio/fanfare.wav");
	//audio_->SoundPlayWave(audioData_, true, 3.0f);
	sprite_.reset(Sprite::Create("resources/clear.png"));
	size = {1.0f, 1.0f};

	viewProjection_.Initialize();
	viewProjection_.rotation_.x = 0.28f;
	viewProjection_.translation_ = { 0.0f, 7.0f, -18.0f };
	worldTransformSphere_.Initialize();
	worldTransformGround_.Initialize();
	worldTransformGround_.rotate.y = 1.58f;
	// スフィア
	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/monsterBall.png"));
	
	modelBunny_.reset(Model::CreateModelFromObj("resources/bunny.obj", "resources/moon.png"));
	modelGround_.reset(Model::CreateModelFromObj("resources/terrain/terrain.obj", "resources/terrain/grass.png"));

}

void TitleScene::Update() {
	

	if (KeyInput::PushKey(DIK_P)) {
		SceneManager::GetInstance()->ChangeScene("GameScene");
	}
	sprite_->SetSize(size);
	sphere_->SetShininess(70.0f);

	if (KeyInput::GetKey(DIK_A)) {
		pointLight_.position_.x += -0.1f;
	}
	else if (KeyInput::GetKey(DIK_D)) {
		pointLight_.position_.x += 0.1f;
	}
	if (KeyInput::GetKey(DIK_S)) {
		pointLight_.position_.y += -0.1f;
	}
	else if (KeyInput::GetKey(DIK_W)) {
		pointLight_.position_.y += 0.1f;
	}



	worldTransformSphere_.UpdateMatrix();
	worldTransformGround_.UpdateMatrix();
	viewProjection_.UpdateMatrix();

	const char* items[] = { "DirectionLight", "PointLight", "SpotLight" };
	static int currentItem = 1; // 初期選択アイテムのインデックス


	if (currentItem == 0) {
		Model::DirectionalLightDraw(directionLight_);
	}
	else if (currentItem == 1) {
		Model::PointLightDraw(pointLight_,directionLight_.direction);
	}
	else if (currentItem == 2) {
		Model::SpotLightDraw(spotLight_);
	}

	ImGui::Begin("Setting");
	if (ImGui::TreeNode("Bunny")) {

		ImGui::SliderFloat3("BunnyPos", &worldTransformSphere_.translate.x, -10.0f, 10.0f);
		ImGui::SliderFloat3("BunnyRotate", &worldTransformSphere_.rotate.x, -1.0f, 1.0f);
		ImGui::SliderFloat3("BunnySize", &worldTransformSphere_.scale.x, -10.0f, 10.0f);
		
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Light")) {
		// LightLight
		ImGui::Combo("##combo", &currentItem, items, IM_ARRAYSIZE(items));
		if (ImGui::TreeNode("DirectionLight")) {
			ImGui::SliderFloat4("DirectionLightColor", &directionLight_.color.x, -1.0f, 1.0f);
			ImGui::DragFloat3("DirectionLightDirecton", &directionLight_.direction.x, 0.1f);
			ImGui::DragFloat("DirectionIntensity", &directionLight_.intensity, 0.1f);
			//ImGui::Checkbox("DirectionLight%d", &directionLight_.isEnable_);
			ImGui::TreePop();
		}
		
		if (ImGui::TreeNode("PointLight")) {
			// LightLight
			ImGui::SliderFloat4("PointLightColor", &pointLight_.color_.x, -1.0f, 1.0f);
			ImGui::DragFloat3("PointLightPosition", &pointLight_.position_.x, 0.1f);
			ImGui::DragFloat("PointRadius", &pointLight_.radius_, 0.1f);
			ImGui::DragFloat("pointDecay", &pointLight_.decay_, 0.1f);
			ImGui::DragFloat("PointIntensity", &pointLight_.intensity_, 0.1f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("spotLight")) {
			// LightLight
			ImGui::SliderFloat4("SpotLightColor", &spotLight_.color_.x, -1.0f, 1.0f);
			ImGui::DragFloat3("SpotLightPosition", &spotLight_.position_.x, 0.1f);
			ImGui::DragFloat3("SpotLightDirecton", &spotLight_.direction_.x, 0.1f);
			ImGui::DragFloat("SpotDistance", &spotLight_.distance_, 0.1f);
			ImGui::DragFloat("SpotDecay", &spotLight_.cosAngle_, 0.1f);
			ImGui::DragFloat("SpotAngle", &spotLight_.decay_, 0.1f);
			ImGui::DragFloat("SpotIntensity", &spotLight_.intensity_, 0.1f);

			ImGui::TreePop();
		}


		
		ImGui::TreePop();
	}

	ImGui::End();
}

void TitleScene::Draw() {
	// 3Dオブジェクト描画前処理



	Transform uv;
	uv.scale = {0.0f, 0.0f, 0.0f};
	uv.rotate = {0.0f, 0.0f, 0.0f};
	uv.translate = {0.0f, 0.0f};
	modelBunny_->Draw(worldTransformSphere_, viewProjection_, true);
	modelGround_->Draw(worldTransformGround_, viewProjection_, true);
	ImGui::Begin("scene");
	ImGui::SliderFloat3("s", &uv.scale.x, 0.0f, 10.0f);
	ImGui::End();
	sprite_->Draw(uv);
	//sphere_->Draw(worldTransformSphere_, viewProjection_, true);
}