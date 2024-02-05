#include "GameScene.h"
#include "TextureManeger.h"
#include "Framework/SceneManager.h"
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
	worldTransformSphere_.Initialize();
	worldTransformSphere_.scale = {3.0f, 3.0f, 3.0f};

	viewProjection_.Initialize();
	viewProjection_.rotation_.x = 0.28f;
	viewProjection_.translation_ = {0.0f, 10.0f, -25.0f};

	
	worldTransform_.translate.x = {0.0f};
	worldTransform_.translate.z = {0.0f};
	worldTransform_.scale = {1.0f, 1.0f, 1.0f};
	for (int i = 0; i < 2; i++) {
		worldTransformFence_[i].Initialize();
		
	}
	worldTransformFence_[0].rotate.y = 1.58f;
	modelFence_.reset(Model::CreateModelFromObj("resources/terrain/terrain.obj", "resources/terrain/grass.png"));

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
	particle2_.reset(Particle::Create("resources/moon.png", emitter_));


	//sprite_.reset(Sprite::Create("resources/uvChecker.png"));
	
	
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
	
	particle2_->SetTranslate({-10.0f,10.0});
	particle2_->SetSpeed(float(num));
	particle2_->Update();

	if (KeyInput::PushKey(DIK_P)) {
		SceneManager::GetInstance()->ChangeScene("ClearScene");
	}

	if (KeyInput::PushKey(DIK_O)) {
		SceneManager::GetInstance()->ChangeScene("OverScene");
	}

	

	worldTransform_.rotate.y += 0.02f;
	worldTransform_.UpdateMatrix();
	worldTransformp_.UpdateMatrix();
	worldTransformSphere_.UpdateMatrix();
	worldTransformFence_[0].UpdateMatrix();
	viewProjection_.UpdateMatrix();
	skydome_->Update();


	modelplane_->SetColor(colorPlane);
	
	
	ImGui::Begin("Setting");
	

	
	if (ImGui::TreeNode("Particle")) {
		ImGui::Checkbox("move", &particle);
		ImGui::Checkbox("area", &area);
		ImGui::SliderInt("speed", &num, 0, 100);
		ImGui::SliderFloat3("Pos", &pos.x, 0.0f, 10.0f);
		ImGui::SliderFloat4("Color", &particleColor.x, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	
	
	ImGui::End();
	
	ImGui::Begin("GameScene");
	ImGui::Text("GameClearScene : push::P");
	ImGui::Text("GameOverScene : push::O");
	ImGui::End();
}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	//modelFence_->LightDraw(color_,direction_, intensity_);

	// 天球
	skydome_->Draw(viewProjection_,false);
	
	//フェンス
	modelFence_->Draw(worldTransformFence_[0], viewProjection_, true);
	
	//パーティクル
	particle_->Draw(viewProjection_);
	particle2_->Draw(viewProjection_);
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}