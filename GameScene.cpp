#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include "externals/imgui/imgui.h"


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

	modelplane_.reset(
	    Model::CreateModelFromObj("resources/plane.obj", "resources/uvChecker.png"));
	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(
	    Model::CreateModelFromObj("resources/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());

	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/monsterBall.png"));
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
	worldTransform_.UpdateMatrix();
	worldTransformp_.UpdateMatrix();
	viewProjection_.UpdateMatrix();
	skydome_->Update();

	ImGui::Begin("Setting");
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

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 天球
	skydome_->Draw(viewProjection_,false);
	modelplane_->Draw(worldTransformp_, viewProjection_,true);
	//modelplane_->Draw(worldTransform_, viewProjection_, false);
	sphere_->Draw(worldTransform_, viewProjection_,true);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();
	
	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	//sprite_->Draw(worldTransformA_);
	
	//sprite2_->Draw(worldTransformB_);
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}