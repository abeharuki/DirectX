#include "TitleScene.h"
#include "Framework/SceneManager.h"

void TitleScene::Initialize() { 
	audio_ = Audio::GetInstance();
	//audioData_ = audio_->SoundLoadWave("resources/audio/fanfare.wav");
	audioData_ = audio_->SoundLoadMP3("resources/audio/BGM.mp3");
	audio_->SoundPlayMP3(audioData_, true, 3.0f);
	sprite_.reset(Sprite::Create("resources/uvChecker.png"));
	startPos_ = { 0.0f,0.0f,0.0f };
	endPos_ = { 0.0f,1.0f,0.0f };
	line_.reset(Line::CreateLine(startPos_, endPos_));
	viewProjection_.Initialize();
	viewProjection_.rotation_.x = 0.28f;
	viewProjection_.translation_ = { 0.0f, 7.0f, -18.0f };
	worldTransformSphere_.Initialize();
	//worldTransformSphere_.scale = { 10.0f,10.0f,10.0f };
	worldTransformBox_.Initialize();
	worldTransformBox_.rotate.y = 3.0f;
	worldTransformGround_.Initialize();
	worldTransformGround_.rotate.y = 1.58f;
	// スフィア
	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/monsterBall.png"));
	
	//animation_ = std::make_unique<Animations>();
	//animation_.reset(Animations::Create("./resources/AnimatedCube", "AnimatedCube_BaseColor.png","AnimatedCube.gltf"));

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("resources/Enemy", "enemy.png", "enemy.gltf"));

	modelBunny_.reset(Model::CreateModelFromObj("resources/bunny.obj", "resources/moon.png"));
	modelGround_.reset(Model::CreateModelFromObj("resources/terrain/terrain.obj", "resources/terrain/grass.png"));
	isSprite_ = false;
	uv.scale = { 0.0f, 0.0f, 0.0f };
	uv.rotate = { 0.0f, 0.0f, 0.0f };
	uv.translate = { 0.0f, 0.0f };
}

void TitleScene::Update() {
	
	

	if (Input::PushKey(DIK_P)) {
		SceneManager::GetInstance()->ChangeScene("GameScene");
	}

	if (Input::PushKey(DIK_L)) {
		SceneManager::GetInstance()->ChangeScene("LoadScene");
	}
	

	sprite_->SetSize(spriteSize_);
	sprite_->SetPosition(spritePos_);

	if (Input::PressKey(DIK_A)) {
		pointLight_.position_.x += -0.1f;
	}
	else if (Input::PressKey(DIK_D)) {
		pointLight_.position_.x += 0.1f;
	}
	if (Input::PressKey(DIK_S)) {
		pointLight_.position_.y += -0.1f;
	}
	else if (Input::PressKey(DIK_W)) {
		pointLight_.position_.y += 0.1f;
	}
	line_->SetLinePos(startPos_, endPos_);
	//worldTransformBox_.rotate.y += 0.01f;
	worldTransformSphere_.UpdateMatrix();
	worldTransformGround_.UpdateMatrix();
	worldTransformBox_.UpdateMatrix();
	viewProjection_.UpdateMatrix();
	animation_->Update(worldTransformBox_);
	
	const char* items[] = { "DirectionLight", "PointLight", "SpotLight" };
	static int currentItem = 1; // 初期選択アイテムのインデックス


	if (currentItem == 0) {
		Model::DirectionalLightDraw(directionLight_);
		Sphere::DirectionalLightDraw(directionLight_);
		Animations::DirectionalLightDraw(directionLight_);
	}
	else if (currentItem == 1) {
		Model::PointLightDraw(pointLight_, directionLight_.direction);
		Sphere::PointLightDraw(pointLight_, directionLight_.direction);
		Animations::PointLightDraw(pointLight_, directionLight_.direction);
	}
	else if (currentItem == 2) {
		Model::SpotLightDraw(spotLight_);
		Sphere::SpotLightDraw(spotLight_);
		Animations::SpotLightDraw(spotLight_);
	}

	ImGui::Begin("Setting");

	if (ImGui::TreeNode("Line")) {
		ImGui::DragFloat3("startLine", &startPos_.x, 0.01f);
		ImGui::DragFloat3("endLine", &endPos_.x, 0.01f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Sprite")) {
		ImGui::Checkbox("DrawSprite", &isSprite_);
		ImGui::DragFloat2("SpritePos", &spritePos_.x, 0.1f);
		ImGui::DragFloat2("SprirteSize", &spriteSize_.x, 0.01f);
	
		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Bunny")) {

		ImGui::DragFloat3("BunnyPos", &worldTransformSphere_.translate.x, 0.1f);
		ImGui::DragFloat3("BunnyRotate", &worldTransformSphere_.rotate.x, 0.01f);
		ImGui::DragFloat3("BunnySize", &worldTransformSphere_.scale.x, 0.1f);
		
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Animation")) {

		ImGui::DragFloat3("AnimationPos", &worldTransformBox_.translate.x, 0.1f);
		ImGui::DragFloat3("AnimationRotate", &worldTransformBox_.rotate.x, 0.01f);
		ImGui::DragFloat3("AnimationSize", &worldTransformBox_.scale.x, 0.1f);

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

	animation_->AnimationDebug();

	ImGui::Begin("TitleScene");
	ImGui::Text("GameScene : push::P");
	ImGui::End();

	
}

void TitleScene::Draw() {
	// 3Dオブジェクト描画前処理

	//line_->Draw(worldTransformBox_,viewProjection_,false);
	//human_->Draw(worldTransformSphere_, viewProjection_,true);
	animation_->Draw(worldTransformBox_, viewProjection_,true);
	//modelBunny_->Draw(worldTransformSphere_, viewProjection_, true);
	//modelGround_->Draw(worldTransformGround_, viewProjection_, true);
	if (isSprite_) {
		sprite_->Draw(uv);
	}
	
	//sphere_->Draw(worldTransformSphere_, viewProjection_, true);
}