#include "TutorialScene.h"
#include "Framework/SceneManager.h"

void TutorialScene::Initialize() {
	viewProjection_.Initialize();
	viewProjection_.translation_ = { 0.0f, 1.0f, -10.0f };


	audio_ = Audio::GetInstance();
	audioData_ = audio_->SoundLoadWave("resources/audio/fanfare.wav");

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
	initialPos_ = playerManager_->GetPlayer()->GetWorldPosition();
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
	spriteBack_->SetSize({ 1280.0f,720.0f });
	isFadeIn_ = true;
	isFadeOut_ = false;
	isFede_ = false;
	//チュートリアルフラグ
	move_ = false;
	dash_ = false;
	jump_ = false;
	attack_ = false;
	combo_ = false;
	step2_ = false;
	call_ = false;
	order_ = false;
}

void TutorialScene::Update() {
	spriteBack_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });
	if (Input::GetInstance()->GetPadConnect()) {
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A) && !isFadeIn_) {
			if (call_ && order_) {
				isFadeOut_ = true;
			}

		}
	}

	if (Input::PushKey(DIK_G)) {
		if (call_ && order_) {
			isFadeOut_ = true;
		}
		
	}

	
	Fade();
	playerManager_->Update();
	//チュートリアルの確認
	//移動
	if (playerManager_->GetPlayer()->GetWorldPosition().x != initialPos_.x ||
		playerManager_->GetPlayer()->GetWorldPosition().z != initialPos_.z) {
		move_ = true;
	}
	if (playerManager_->GetPlayer()->IsDash()) {
		dash_ = true;
	}
	//ジャンプ
	if (playerManager_->GetPlayer()->GetWorldPosition().y != initialPos_.y) {
		jump_ = true;
	}
	//攻撃
	if (playerManager_->GetPlayer()->IsAttack()) {
		attack_ = true;
	}
	//コンボ
	if (playerManager_->GetPlayer()->IsCombo()) {
		combo_ = true;
	}

	if (move_ && jump_ && attack_ && combo_) {
		step2_ = true;
	}


	if (step2_ && Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X)) {
		call_ = true;
		
	}


	if (step2_ && Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_Y)) {
		order_ = true;
		
	}

	if (step2_) {
		if (call_ || order_) {
			healerManager_->Update();
			renjuManager_->Update();
			tankManager_->Update();
		}
	}

	cameraMove();
	skydome_->Update();
	ground_->Update();




	if (Input::PushKey(DIK_C)) {
		SceneManager::GetInstance()->ChangeScene("ClearScene");
	}

	if (Input::PushKey(DIK_O)) {
		SceneManager::GetInstance()->ChangeScene("OverScene");
	}


	ImGui::Begin("step");
	ImGui::Text("move%d", move_);
	ImGui::Text("dash%d", dash_);
	ImGui::Text("jump%d", jump_);
	ImGui::Text("attack%d", attack_);
	ImGui::Text("combo%d", combo_);
	ImGui::Text("call%d", call_);
	ImGui::Text("order%d", order_);
	ImGui::End();

}

void TutorialScene::Draw() {
	// 3Dオブジェクト描画前処理
	//Model::LightDraw(color_, direction_, intensity_);

	// 天球
	skydome_->Draw(viewProjection_, false);
	// 地面
	ground_->Draw(viewProjection_, false);

	// プレイヤー
	playerManager_->Draw(viewProjection_);
	// 敵
	//enemyManager_->Draw(viewProjection_);
	if (step2_) {
		// タンク
		tankManager_->Draw(viewProjection_);
		// ヒーラー
		healerManager_->Draw(viewProjection_);
		// レンジャー
		renjuManager_->Draw(viewProjection_);

	}
	
	Transform uv;
	uv.scale = { 0.0f, 0.0f, 0.0f };
	uv.rotate = { 0.0f, 0.0f, 0.0f };
	uv.translate = { 0.0f, 0.0f, 0.0f };
	spriteBack_->Draw(uv);
}

void TutorialScene::cameraMove() {

	// 追従カメラの更新
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

}


void TutorialScene::Fade() {
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
			SceneManager::GetInstance()->ChangeScene("GameScene");
		}
	}
}