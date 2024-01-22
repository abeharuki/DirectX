#include "TitleScene.h"


void TitleScene::Initialize() { 
	audio_ = Audio::GetInstance();
	audioData_ = audio_->SoundLoadWave("resources/audio/fanfare.wav");
	//audio_->SoundPlayWave(audioData_, true, 3.0f);
	sprite_.reset(Sprite::Create("resources/clear.png"));
	size = {1.0f, 1.0f};

	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -10.0f};
	worldTransformSphere_.Initialize();
	// スフィア
	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/monsterBall.png"));
	
}

void TitleScene::Update() {
	
	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = GAME;
	}
	sprite_->SetSize(size);
	sphere_->SetShininess(70.0f);

	worldTransformSphere_.UpdateMatrix();
	viewProjection_.UpdateMatrix();

	ImGui::Begin("Setting");
	if (ImGui::TreeNode("Sphere")) {

		ImGui::SliderFloat3("SpritePos", &worldTransformSphere_.translate.x, -10.0f, 10.0f);
		ImGui::SliderFloat3("SpriteRotate", &worldTransformSphere_.rotate.x, -10.0f, 10.0f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Light")) {
		// LightLight
		ImGui::SliderFloat3("LightColor", &color_.x, -1.0f, 1.0f);
		ImGui::SliderFloat3("LightDirecton", &direction_.x, -1.0f, 1.0f);
		ImGui::DragFloat("Intensity", &intensity_, 0.1f);

		ImGui::TreePop();
	}

	ImGui::End();
}

void TitleScene::Draw() {
	Transform uv;
	uv.scale = {0.0f, 0.0f, 0.0f};
	uv.rotate = {0.0f, 0.0f, 0.0f};
	uv.translate = {0.0f, 0.0f};
	// 3Dオブジェクト描画前処理
	Sphere::LightDraw(color_, direction_, intensity_);


	ImGui::Begin("scene");
	ImGui::SliderFloat3("s", &uv.scale.x, 0.0f, 10.0f);
	ImGui::End();
	sprite_->Draw(uv);
	sphere_->Draw(worldTransformSphere_, viewProjection_, true);
}