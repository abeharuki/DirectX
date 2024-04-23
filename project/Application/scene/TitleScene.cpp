#include "TitleScene.h"
#include "Framework/SceneManager.h"


void TitleScene::Initialize() {
	worldTransform_.Initialize();
	worldTransform_.scale = { 10.0f,10.0f,10.0f };
	worldTransform_.translate.y = 5;
	//worldTransform_.rotate = {0.7f,0.0f,3.14f};
	viewProjection_.Initialize();
	viewProjection_.translation_ = { 0.0f, 1.0f, -10.0f };

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/AnimatedCube", "tex.png", "bound3.gltf"));



	audio_ = Audio::GetInstance();
	audioData_[0] = audio_->SoundLoadMP3("resources/audio/BGM.mp3");
	audio_->SoundPlayMP3(audioData_[1], true, 3.0f);

	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(
		Model::CreateModelFromObj("resources/skydome/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());

	// 地面
	ground_ = std::make_unique<Ground>();
	// 3Dモデルの生成
	modelGround_.reset(
		Model::CreateModelFromObj("resources/ground/ground.obj", "resources/ground/ground.png"));
	ground_->Initialize(
		Model::CreateModelFromObj("resources/ground/ground.obj", "resources/ground/ground.png"));

	// プレイヤー
	playerManager_ = std::make_unique<PlayerManager>();
	playerManager_->Initialize();

	// 追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&playerManager_->GetWorldTransform());

	// 自キャラの生成と初期化処理
	playerManager_->SetViewProjection(&followCamera_->GetViewProjection());

	// 敵
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize();

	// タンク
	tankManager_ = std::make_unique<TankManager>();
	tankManager_->Initialize();
	// レンジャー
	renjuManager_ = std::make_unique<RenjuManager>();
	renjuManager_->Initialize();
	// ヒーラー
	healerManager_ = std::make_unique<HealerManager>();
	healerManager_->Initialize();

	alpha_ = 1.0f;
	// フェードイン・フェードアウト用スプライト
	spriteBack_.reset(Sprite::Create("resources/Player/B.png"));
	spriteBack_->SetSize({ 10.0f, 10.0f });
	spriteTitle_.reset(Sprite::Create("resources/Title/title.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/starte.png"));
	//spriteRule_.reset(Sprite::Create("resources/Title/rule.png"));
	rule_ = false;
	pos_.x = 1280.0f;
	spriteTitle_->SetPosition({-250,0.0f});
	//spritePushA_->SetSize({ 3.6f, 2.0f });
	//spriteRule_->SetSize({ 1280.0f, 720.0f });
	spriteBack_->SetSize({ 1280.0f,720.0f });
	isFadeIn_ = true;
	isFadeOut_ = false;
	isFede_ = false;
}

void TitleScene::Update() {

	

	spriteBack_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });
	if (Input::GetInstance()->GetPadConnect()) {
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			//rule_ = true;
		}
	}

	if (Input::PushKey(DIK_G)) {
		//rule_ = true;
	}
	/*
	if (rule_) {
		const float speed = 30.0f;
		if (pos_.x > 0.0f) {
			pos_.x -= speed;
		}
		else {
			pos_.x = 0.0f;
			
		}

	}*/

	if (Input::GetInstance()->GetPadConnect()) {
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A) && !isFadeIn_) {
			isFadeOut_ = true;
		}

		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B) && !isFadeIn_) {
			rule_ = true;
			isFadeOut_ = true;
		}

	}

	if (Input::PushKey(DIK_G) && !isFadeIn_) {
		isFadeOut_ = true;
	}

	if (Input::PushKey(DIK_H) && !isFadeIn_) {
		rule_ = true;
		isFadeOut_ = true;
	}




	Fade();
	cameraMove();
	skydome_->Update();
	ground_->Update();

	animation_->Update(worldTransform_,false);

	if (Input::PushKey(DIK_C)) {
		SceneManager::GetInstance()->ChangeScene("ClearScene");
	}

	if (Input::PushKey(DIK_O)) {
		SceneManager::GetInstance()->ChangeScene("OverScene");
	}
	ImGui::Begin("Player");
	ImGui::SliderFloat3("pos", &worldTransform_.translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("rotate", &worldTransform_.rotate.x, -0.0f, 10.0f);
	ImGui::End();
}

void TitleScene::Draw() {
	// 3Dオブジェクト描画前処理
	//Model::LightDraw(color_, direction_, intensity_);

	// 天球
	skydome_->Draw(viewProjection_, false);
	// 地面
	ground_->Draw(viewProjection_, false);

	// プレイヤー
	playerManager_->Draw(viewProjection_);
	// 敵
	enemyManager_->Draw(viewProjection_);
	// タンク
	tankManager_->Draw(viewProjection_);
	// ヒーラー
	healerManager_->Draw(viewProjection_);
	// レンジャー
	renjuManager_->Draw(viewProjection_);

	animation_->Draw(worldTransform_, viewProjection_);

	Transform uv;
	uv.scale = { 0.0f, 0.0f, 0.0f };
	uv.rotate = { 0.0f, 0.0f, 0.0f };
	uv.translate = { 0.0f, 0.0f, 0.0f };
	spriteTitle_->Draw(uv);
	spritePushA_->Draw(uv);
	//spriteRule_->Draw(uv);
	spriteBack_->Draw(uv);
}

void TitleScene::cameraMove() {

	// 追従カメラの更新
	followCamera_->TitleUpdate();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

}


void TitleScene::Fade() {
	if (isFadeIn_) {
		if (alpha_ > 0.0f) {
			alpha_ -= 0.02f;
		}
		else {
			alpha_ = 0.0f;
			isFadeIn_ = false;
		}
	}

	if (isFadeOut_) {
		if (alpha_ < 1) {
			alpha_ += 0.02f;
		}
		else {
			alpha_ = 1.0f;
			if (rule_) {
				SceneManager::GetInstance()->ChangeScene("TutorialScene");
			}
			else {
				SceneManager::GetInstance()->ChangeScene("GameScene");
			}
			
		}
	}
}