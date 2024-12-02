#include "TitleScene.h"
#include "Framework/SceneManager.h"



void TitleScene::Initialize() {
	worldTransform_.Initialize();
	worldTransform_.scale = { 10.0f,10.0f,10.0f };
	worldTransform_.translate.z = -5;
	viewProjection_.Initialize();
	viewProjection_.translation_ = { 0.0f, 1.0f, -10.0f };
	
	/*audio_ = Audio::GetInstance();
	audioData_[0] = audio_->SoundLoadMP3("resources/audio/BGM.mp3");
	audio_->SoundPlayMP3(audioData_[1], true, 3.0f);*/

	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(
		Model::CreateModelFromObj("resources/skydome/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());

	// 地面
	loader_.reset(ModelLoader::Create("resources/JsonFile/loader.json"));

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
	spriteBack_.reset(Sprite::Create("resources/Black.png"));
	spriteTitle_.reset(Sprite::Create("resources/Title/DRAPONQUEST1.png"));
	spriteTitle_->SetSize({ 1280.0f,905.0f });
	spriteTitle_->SetPosition({ 0.0f,-250.0f });
	spritePushA_.reset(Sprite::Create("resources/Title/starte.png"));
	spritePushA_->SetPosition(Vector2{ 50.0f,0.0f });
	//spriteRule_.reset(Sprite::Create("resources/Title/rule.png"));
	rule_ = false;
	
	spriteBack_->SetSize({ 1280.0f,720.0f });
	isFadeIn_ = true;
	isFadeOut_ = false;
	isFede_ = false;

	a_ = 0.005f;

	PostEffect::GetInstance()->isGrayscale(false);

#ifndef NDEBUG
#else
	// リリースビルド時のコード
	PostEffect::GetInstance()->isOutLine(true);
	PostEffect::GetInstance()->isBloom(true);
#endif
	
	
	
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
	

	enemyManager_->Update();

	Fade();
	cameraMove();
	skydome_->Update();
	loader_->Update();
	worldTransform_.UpdateMatrix();
	
	//ライティングの設定
	playerManager_->GetPlayer()->SetLight(directionLight_);
	enemyManager_->GetEnemy()->SetLight(directionLight_);
	healerManager_->GetHealer()->SetLight(directionLight_);
	renjuManager_->GetRenju()->SetLight(directionLight_);
	tankManager_->GetTank()->SetLight(directionLight_);
	loader_->SetLight(directionLight_);

	PostEffect::GetInstance()->ValueOutLine(a_);

	ImGui::Begin("Player");
	ImGui::SliderFloat3("pos", &worldTransform_.translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("rotate", &worldTransform_.rotate.x, -0.0f, 10.0f);
	ImGui::End();

	ImGui::Begin("Setting");
	ImGui::DragFloat3("DirectionLight", &directionLight_.direction.x, 1.0f);
	ImGui::DragFloat("Light", &directionLight_.intensity, 1.0f);
	ImGui::DragFloat("outline", &a_, 0.01f);
	ImGui::DragFloat2("TitlePos", &pos_.x, 1.0f);
	ImGui::End();

	editor_.load("Example");
	editor_.show("ExampleNode");
	editor_.save("Example");
	
	//std::string state = editor_.GetLinkNode("Attack", Output1).name;

}

void TitleScene::Draw() {
	// 3Dオブジェクト描画前処理
	
	// 天球
	skydome_->Draw(viewProjection_, false);

	// 地面
	loader_->Draw(viewProjection_, true);

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

	spriteTitle_->Draw();
	spritePushA_->Draw();
	
}

void TitleScene::RenderDirect() {
	
	spriteBack_->Draw();
}

void TitleScene::cameraMove() {

	// 追従カメラの更新
	followCamera_->TitleUpdate();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

}


void TitleScene::Fade() {
	if (Input::GetInstance()->GetPadConnect()) {
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A) && !isFadeIn_) {
			isFadeOut_ = true;
		}
	}

	if (Input::PushKey(DIK_SPACE) && !isFadeIn_) {
		isFadeOut_ = true;
	}

	//フェードイン時
	if (isFadeIn_) {
		if (alpha_ > 0.0f) {
			alpha_ -= 0.02f;
		}
		else {
			alpha_ = 0.0f;
			isFadeIn_ = false;
		}
	}

	//フェードアウト時
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