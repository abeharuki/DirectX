#include "TitleScene.h"
#include "Framework/SceneManager.h"



void TitleScene::Initialize() {
	viewProjection_.Initialize();
	
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

	
	spriteTitle_.reset(Sprite::Create("resources/Title/DRAPONQUEST1.png"));
	spriteTitle_->SetSize(TitleSceneConstants::kSpriteTitleSize);
	spriteTitle_->SetPosition(TitleSceneConstants::kSpriteTitlePos);
	spritePushA_.reset(Sprite::Create("resources/Title/starte.png"));
	spritePushA_->SetPosition(TitleSceneConstants::kSpritePushAPos);
	spritePushA_->SetSize({ 1280,720 });
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	PostEffect::GetInstance()->isGrayscale(false);
	PostEffect::GetInstance()->ValueOutLine(TitleSceneConstants::kOutLineValue);

	PostEffect::GetInstance()->isOutLine(true);
	PostEffect::GetInstance()->isBloom(true);
	
	
}

void TitleScene::Update() {
	
	

	enemyManager_->Update();
	
	//シーン遷移
	transition_->ChangeScene();
	transition_->Update();

	if (transition_->GetFade()) {
		SceneManager::GetInstance()->ChangeScene("GameScene");
	}
	
	cameraMove();
	skydome_->Update();
	loader_->Update();
	
	
	//ライティングの設定
	playerManager_->GetPlayer()->SetLight(directionLight_);
	enemyManager_->GetEnemy()->SetLight(directionLight_);
	healerManager_->GetHealer()->SetLight(directionLight_);
	renjuManager_->GetRenju()->SetLight(directionLight_);
	tankManager_->GetTank()->SetLight(directionLight_);
	loader_->SetLight(directionLight_);

	

	

	ImGui::Begin("Setting");
	ImGui::DragFloat3("DirectionLight", &directionLight_.direction.x, 1.0f);
	ImGui::DragFloat("Light", &directionLight_.intensity, 1.0f);
	ImGui::End();

	editor_.load("Example");
	editor_.show("ExampleNode");
	editor_.save("Example");
	
	
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

	
	
}

void TitleScene::RenderDirect() {
	spriteTitle_->Draw();
	spritePushA_->Draw();
	transition_->Draw();
}

void TitleScene::cameraMove() {

	// 追従カメラの更新
	followCamera_->TitleUpdate();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

}


