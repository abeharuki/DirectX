#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>


GameScene::GameScene() {}

GameScene::~GameScene() { 
	

}

GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize() {
	worldTransform_.Initialize();
	worldTransformp_.Initialize();
	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -5.0f};

	worldTransform_.translate.x = {1.0f};
	worldTransform_.translate.z = {-2.0f};
	worldTransform_.scale = {0.5f, 0.5f, 0.5f};
	for (int i = 0; i < 2; i++) {
		worldTransformFence_[i].Initialize();
		modelFence_[i].reset(
		    Model::CreateModelFromObj("resources/fence/fence.obj", "resources/fence/fence.png"));
	}
	
	//板ポリ
	modelplane_.reset(
	    Model::CreateModelFromObj("resources/plane.obj", "resources/uvChecker.png"));
	
	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(
	    Model::CreateModelFromObj("resources/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());
	//スフィア
	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/monsterBall.png"));

	particle_.reset(Particle::Create("resources/uvChecker.png",10));


	sprite_.reset(Sprite::Create("resources/uvChecker.png"));

	
	particle = false;
	colorPlane = {1.0f, 1.0f, 1.0f, 1.0f};
	blendMode_ = BlendMode::kNone;
	
}

void GameScene::Update() {

	if (KeyInput::GetKey(DIK_W)) {
		worldTransformp_.translate.z += 0.1f;
	} else if (KeyInput::GetKey(DIK_S)) {
		worldTransformp_.translate.z -= 0.1f;
	}

	if (KeyInput::GetKey(DIK_A)) {
		worldTransformp_.translate.x -= 0.1f;
	} else if (KeyInput::GetKey(DIK_D)) {
		worldTransformp_.translate.x += 0.1f;
	}

	if (KeyInput::GetKey(DIK_RIGHTARROW)) {
		colorPlane.w += 0.01f;
	} else if (KeyInput::GetKey(DIK_LEFTARROW)) {
		colorPlane.w -= 0.01f;
	}

	if (particle) {
		//particle_->Update();
	}
	

	worldTransform_.rotate.y += 0.02f;
	worldTransform_.UpdateMatrix();
	worldTransformp_.UpdateMatrix();
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

	
	if (ImGui::TreeNode("Sphere")) {
		
		;
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
		// LightLight
		ImGui::Checkbox("move", &particle);
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
	modelplane_->SetBlendMode(blendMode_);
}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::LightDraw(color_,direction_, intensity_);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 天球
	skydome_->Draw(viewProjection_,false);
	//sphere_->Draw(worldTransform_, viewProjection_,true);
	//フェンス
	modelFence_[0]->Draw(worldTransformFence_[0], viewProjection_, false);
	//板ポリ
	//modelplane_->Draw(worldTransformp_, viewProjection_, true);
	//パーティクル
	particle_->Draw(worldTransformp_, viewProjection_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();
	
	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	//sprite_->Draw(worldTransform_,uvTransform_);
	
	//sprite2_->Draw(worldTransformB_);
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}