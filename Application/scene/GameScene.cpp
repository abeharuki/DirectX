#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>
#include <numbers>


GameScene::GameScene() {}

GameScene::~GameScene() {}

GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize() {
	worldTransform_.Initialize();
	worldTransformp_.Initialize();
	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -10.0f};
	
	worldTransform_.translate.x = {0.0f};
	worldTransform_.translate.z = {0.0f};
	worldTransform_.scale = {1.0f, 1.0f, 1.0f};
	for (int i = 0; i < 2; i++) {
		worldTransformFence_[i].Initialize();
		modelFence_[i].reset(
		    Model::CreateModelFromObj("resources/bunny.obj", "resources/moon.png"));
	}
	
	//板ポリ
	modelplane_.reset(
	    Model::CreateModelFromObj("resources/plane.obj", "resources/uvChecker.png"));
	
	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(Model::CreateModelFromObj("resources/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());

	//スフィア
	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/moon.png"));

	emitter_.transform = {
	    {1.0f, 1.0f, 1.0f},
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, 0.0f},
	};
	emitter_.count = 10;
	emitter_.frequency = 0.1f;
	emitter_.frequencyTime = 0.0f;

	accelerationField_.acceleration = {10.0f, 0.0f, 0.0f};
	accelerationField_.area.min = {-1.0f, -1.0f, -1.0f};
	accelerationField_.area.max = {1.0f, 1.0f, 1.0f};
	particle_.reset(Particle::Create("resources/particle/circle.png",emitter_));


	sprite_.reset(Sprite::Create("resources/uvChecker.png"));

	
	particle = false;
	area = false;
	colorPlane = {1.0f, 1.0f, 1.0f, 1.0f};
	blendMode_ = BlendMode::kNone;
	
}

void GameScene::Update() {

	if (KeyInput::GetKey(DIK_W)) {
		viewProjection_.rotation_.x -= 0.1f;

		//worldTransformp_.translate.z += 0.1f;
	} else if (KeyInput::GetKey(DIK_S)) {
		viewProjection_.rotation_.x += 0.1f;
		//worldTransformp_.translate.z -= 0.1f;
	}

	if (KeyInput::GetKey(DIK_A)) {
		worldTransformp_.translate.x -= 0.1f;
	} else if (KeyInput::GetKey(DIK_D)) {
		worldTransformp_.translate.x += 0.1f;
	}

	if (KeyInput::GetKey(DIK_RIGHTARROW)) {
		//colorPlane.w += 0.01f;
		viewProjection_.translation_.x += 0.1f;
	} else if (KeyInput::GetKey(DIK_LEFTARROW)) {
		viewProjection_.translation_.x -= 0.1f;
		//colorPlane.w -= 0.01f;
	}


	if (KeyInput::GetKey(DIK_UPARROW)) {
		
		viewProjection_.translation_.y += 0.1f;
	} else if (KeyInput::GetKey(DIK_DOWNARROW)) {
		viewProjection_.translation_.y -= 0.1f;
		
	}

	particle_->SetTranslate({pos});
	particle_->SetSpeed(float(num));
	if (particle) {
		particle_->Update();
		particle_->SetFiled(accelerationField_);
		if (area) {
			accelerationField_.acceleration = {15.0f, 0.0f, 0.0f};
		} else {
			accelerationField_.acceleration = {0.0f, 0.0f, 0.0f};
		}
		//particle_->SetColor(particleColor);
	} else {
		particle_->StopParticles();
	}
	
	

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = CLEAR;
	}

	

	worldTransform_.rotate.y += 0.02f;
	worldTransform_.UpdateMatrix();
	worldTransformp_.UpdateMatrix();
	worldTransformFence_[0].UpdateMatrix();
	viewProjection_.UpdateMatrix();
	skydome_->Update();


	modelplane_->SetColor(colorPlane);
	
	
	ImGui::Begin("Setting");
	if (ImGui::TreeNode("plane")) {
		ImGui::SliderFloat4("Color", &colorPlane.x, -1.0f, 1.0f);
		if (ImGui::BeginCombo("BlendMode", EnumToString(blendMode_))) {
			if (ImGui::Selectable("kNone", blendMode_ == BlendMode::kNone)) {
				blendMode_ = BlendMode::kNone;
				
			}
			if (ImGui::Selectable("kNormal", blendMode_ == BlendMode::kNormal)) {
				blendMode_ = BlendMode::kNormal;
			}
			if (ImGui::Selectable("kAdd", blendMode_ == BlendMode::kAdd)) {
				blendMode_ = BlendMode::kAdd;
			}
			if (ImGui::Selectable("kSubtract", blendMode_ == BlendMode::kSubtract)) {
				blendMode_ = BlendMode::kSubtract;

			}
			
			ImGui::EndCombo();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("bunny")) {

		ImGui::SliderFloat3("transletion", &worldTransformFence_[0].translate.x, -1.0f, 10.0f);
		ImGui::SliderFloat3("rotate", &worldTransformFence_[0].rotate.x, -1.0f, 10.0f);

		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Sphere")) {
		
		ImGui::SliderFloat3("Sprite", &worldTransform_.translate.x, -1.0f, 10.0f);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Sprite")) {
		// LightLight
		ImGui::SliderFloat3("UVTransform.scale", &uvTransform_.scale.x, -1.0f, 1.0f);
		ImGui::SliderFloat3("UVTransform.translate", &uvTransform_.translate.x, -1.0f, 1.0f);
		ImGui::SliderFloat3("Sprite", &worldTransform_.scale.x, -1.0f, 10.0f);
		

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Particle")) {
		ImGui::Checkbox("move", &particle);
		ImGui::Checkbox("area", &area);
		ImGui::SliderInt("speed", &num, 0, 100);
		ImGui::SliderFloat3("Pos", &pos.x, 0.0f, 10.0f);
		ImGui::SliderFloat4("Color", &particleColor.x, 0.0f, 1.0f);
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


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::LightDraw(color_,direction_, intensity_);

	// 天球
	skydome_->Draw(viewProjection_,false);
	//sphere_->Draw(worldTransform_, viewProjection_,true);
	//フェンス
	modelFence_[0]->Draw(worldTransformFence_[0], viewProjection_, false);
	//板ポリ
	//modelplane_->Draw(worldTransformp_, viewProjection_, true);
	//パーティクル
	particle_->Draw(viewProjection_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();
	

	//sprite_->Draw(worldTransform_,uvTransform_);
	
	//sprite2_->Draw(worldTransformB_);
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}