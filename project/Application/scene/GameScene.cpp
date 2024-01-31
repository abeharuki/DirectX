#include "GameScene.h"
#include "Framework/SceneManager.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>
#include <numbers>


GameScene::GameScene() {}

GameScene::~GameScene() {}
/*
GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}
*/

void GameScene::Initialize() {

	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -10.0f};
	
	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(Model::CreateModelFromObj("resources/skydome/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());

	// 地面
	ground_ = std::make_unique<Ground>();
	// 3Dモデルの生成
	modelGround_.reset(Model::CreateModelFromObj("resources/ground/ground.obj", "resources/ground/ground.png"));
	ground_->Initialize(
	    Model::CreateModelFromObj("resources/ground/ground.obj", "resources/ground/ground.png"));
	
	alpha_ = 1.0f;
	//フェードイン・フェードアウト用スプライト
	spriteBack_.reset(Sprite::Create("resources/Player/B.png"));
	spriteBack_->SetSize({10.0f, 10.0f});
	
	//プレイヤー
	playerManager_ = std::make_unique<PlayerManager>();
	playerManager_->Initialize();

	//追従カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&playerManager_->GetWorldTransform());

	// 自キャラの生成と初期化処理
	playerManager_->SetViewProjection(&followCamera_->GetViewProjection());

	//敵
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize();

	//タンク
	tankManager_ = std::make_unique<TankManager>();
	tankManager_->Initialize();
	//レンジャー
	renjuManager_ = std::make_unique<RenjuManager>();
	renjuManager_->Initialize();
	//ヒーラー
	healerManager_ = std::make_unique<HealerManager>();
	healerManager_->Initialize();
	
	isFadeIn_ = true;
	isFadeOut_ = false; 

}

void GameScene::Update() {
	
	Fade();
	if (KeyInput::PushKey(DIK_P)) {
		sceneManager_->ChangeScene("ClearScene");
	}
	

	if (enemyManager_->IsClear() && isFadeOut_) {
		sceneManager_->ChangeScene("ClearScene");
	}

	if (playerManager_->IsOver() && isFadeOut_) {
		sceneManager_->ChangeScene("OverScene");
	}
	

	if (!isFadeIn_) {
		
	}
	
	playerManager_->Update();
	healerManager_->Update();
	renjuManager_->Update();
	tankManager_->Update();
	enemyManager_->Update();

	// 追従カメラの更新
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();
	skydome_->Update();
	ground_->Update();
	//プレイヤーに追従
	healerManager_->followPlayer(playerManager_->GetWorldPos());
	renjuManager_->followPlayer(playerManager_->GetWorldPos());
	tankManager_->followPlayer(playerManager_->GetWorldPos());
	//敵の座標の取得
	healerManager_->SetEnemypPos(enemyManager_->GetWorldPos());
	renjuManager_->SetEnemypPos(enemyManager_->GetWorldPos());
	tankManager_->SetEnemypPos(enemyManager_->GetWorldPos());
	//プレイヤーと味方座標の取得
	if (!enemyManager_->IsAttack()) {
		enemyManager_->SetPlayerPos(playerManager_->GetWorldPos());
		enemyManager_->SetHealerPos(healerManager_->GetWorldPos());
		enemyManager_->SetRenjuPos(renjuManager_->GetWorldPos());
		enemyManager_->SetTankPos(tankManager_->GetWorldPos());

	}
	
	CheckAllCollision();
}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::LightDraw(color_,direction_, intensity_);

	// 天球
	skydome_->Draw(viewProjection_,false);
	//地面
	ground_->Draw(viewProjection_,false);
	//プレイヤー
	playerManager_->Draw(viewProjection_);
	//敵
	enemyManager_->Draw(viewProjection_);
	// タンク
	tankManager_->Draw(viewProjection_);
	// ヒーラー
	healerManager_->Draw(viewProjection_);
	// レンジャー
	renjuManager_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();
	
	enemyManager_->DrawUI();
	playerManager_->DrawUI();

	Transform uv;
	uv.scale = {0.0f, 0.0f, 0.0f};
	uv.rotate = {0.0f, 0.0f, 0.0f};
	uv.translate = {0.0f, 0.0f, 0.0f};
	
	spriteBack_->Draw(uv);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

void GameScene::Fade() {
	if (isFadeIn_) {
		if (alpha_ > 0.001f) {
			alpha_ -= 0.02f;
		} else {
			alpha_ = 0.0f;
			isFadeIn_ = false;
		}
	}

	if (playerManager_->IsOver()|| enemyManager_->IsClear()) {
		if (alpha_ < 1) {
			alpha_ += 0.02f;
		} else {
			alpha_ = 1.0f;
			isFadeOut_ = true;
		}
	}
	spriteBack_->SetColor({1.0f, 1.0f, 1.0f, alpha_});
}

void GameScene::CheckAllCollision() {
	// 判定対象AとBの座標
	Vector3 posA, posB;

	const std::list<RenjuBullet*>& renjuBullets = renjuManager_->GetBullets();

#pragma region 敵とレンジャーの弾の当たり判定
	// 敵キャラ座標
	posA = {enemyManager_->GetWorldPos().x, enemyManager_->GetWorldPos().y+6.0f,enemyManager_->GetWorldPos().z};
	//レンジャー弾
	for (RenjuBullet* bullet : renjuBullets) {
		posB = bullet->GetWorldPosition();

		if (Math::IsAABBCollision(posA, {2.0f, 6.0f, 1.0f}, posB, {0.3f, 0.3f, 0.3f})) {
			bullet->OnCollision();
			enemyManager_->OnCollision();
			renjuManager_->SetParticlePos(posB);
		}

	}
	
		
	
#pragma endregion

#pragma region 敵キャラとタンクの当たり判定

	// 敵キャラ座標
	posA = {enemyManager_->GetWorldPos().x, enemyManager_->GetWorldPos().y+6.0f,enemyManager_->GetWorldPos().z};
	posB = tankManager_->GetWorldPos();
	
	if (Math::IsAABBCollision(posA, {2.0f, 6.0f, 1.0f}, posB, {1.0f, 1.0f, 1.0f})) {
		if (tankManager_->GetAttack()) {
			enemyManager_->OnCollision();
			tankManager_->SetParticlePos(posB);
		}
		
	}

#pragma endregion

#pragma region 敵キャラとヒーラーの当たり判定
	// 敵キャラ座標
	posA = {enemyManager_->GetWorldPos().x, enemyManager_->GetWorldPos().y+6.0f,enemyManager_->GetWorldPos().z};
	posB = healerManager_->GetCanePos();

	if (Math::IsAABBCollision(posA, {2.0f, 6.0f, 1.0f}, posB, {0.27f, 0.27f, 1.0f})) {
		if (healerManager_->IsAttack()) {
			enemyManager_->OnCollision();
			healerManager_->SetParticlePos(posB);
		}
		
	}
#pragma endregion

#pragma region 敵キャラと自キャラの攻撃当たり判定
	// 敵キャラ座標
	posA = {enemyManager_->GetWorldPos().x, enemyManager_->GetWorldPos().y+6.0f,enemyManager_->GetWorldPos().z};
	posB = playerManager_->katanaPos();

	
	if (Math::IsAABBCollision(posA, {2.0f, 6.0f, 1.5f}, posB, {0.1f, 1.0f, 0.1f})) {
		if (playerManager_->IsAttack()) {
			enemyManager_->OnCollision();
			playerManager_->SetParticlePos(posB);
		}
		
	}
#pragma endregion

/*------------------
  敵の攻撃の当たり判定
-------------------*/

#pragma region 自キャラと敵キャラ攻撃の当たり判定
	// 敵キャラ座標
	posA = {enemyManager_->GetWorldPos().x, enemyManager_->GetWorldPos().y+6.0f,enemyManager_->GetWorldPos().z};
	posB = playerManager_->GetWorldPos();

	if (Math::IsAABBCollision(posA, {2.0f, 6.0f, 1.5f}, posB, {0.6f, 0.4f, 0.2f})) {
		if (enemyManager_->IsAttack()) {
			playerManager_->OnCollision(enemyManager_->GetWorldTransform());
		}
	}
#pragma endregion
#pragma region ヒーラーと敵キャラ攻撃の当たり判定
	// 敵キャラ座標
	posA = {
	    enemyManager_->GetWorldPos().x, enemyManager_->GetWorldPos().y + 6.0f,
	    enemyManager_->GetWorldPos().z};
	posB = healerManager_->GetWorldPos();

	if (Math::IsAABBCollision(posA, {2.0f, 6.0f, 1.5f}, posB, {0.6f, 0.4f, 0.2f})) {
		if (enemyManager_->IsAttack()) {
			healerManager_->OnCollision(enemyManager_->GetWorldTransform());
			playerManager_->OnHCollision();
		}
	}
#pragma endregion
#pragma region レンジャーと敵キャラ攻撃の当たり判定
	// 敵キャラ座標
	posA = {
	    enemyManager_->GetWorldPos().x, enemyManager_->GetWorldPos().y + 6.0f,
	    enemyManager_->GetWorldPos().z};
	posB = renjuManager_->GetWorldPos();

	if (Math::IsAABBCollision(posA, {2.0f, 6.0f, 1.5f}, posB, {0.6f, 0.4f, 0.2f})) {
		if (enemyManager_->IsAttack()) {
			renjuManager_->OnCollision(enemyManager_->GetWorldTransform());
			playerManager_->OnRCollision();
		}
	}
#pragma endregion
#pragma region タンクと敵キャラ攻撃の当たり判定
	// 敵キャラ座標
	posA = {
	    enemyManager_->GetWorldPos().x, enemyManager_->GetWorldPos().y + 6.0f,
	    enemyManager_->GetWorldPos().z};
	posB = tankManager_->GetWorldPos();

	if (Math::IsAABBCollision(posA, {2.0f, 6.0f, 1.5f}, posB, {0.6f, 0.4f, 0.2f})) {
		if (enemyManager_->IsAttack()) {
			tankManager_->OnCollision(enemyManager_->GetWorldTransform());
			playerManager_->OnTCollision();
		}
	}
#pragma endregion

#pragma region 自キャラと敵キャラ投擲攻撃の当たり判定
	// 敵キャラ座標
	posA = enemyManager_->GetRockWorldPos();
	posB = playerManager_->GetWorldPos();

	if (Math::IsAABBCollision(posA, {1.0f, 1.0f, 1.0f}, posB, {0.6f, 0.4f, 0.2f})) {
		if (enemyManager_->IsAttack()) {
			playerManager_->OnCollision(enemyManager_->GetWorldTransform());
		}
	}
#pragma endregion
#pragma region ヒーラーと敵キャラ投擲攻撃の当たり判定
	// 敵キャラ座標
	posA = enemyManager_->GetRockWorldPos();
	posB = healerManager_->GetWorldPos();

	if (Math::IsAABBCollision(posA, {1.0f,1.0f, 1.0f}, posB, {0.6f, 0.4f, 0.2f})) {
		if (enemyManager_->IsAttack()) {
			healerManager_->OnCollision(enemyManager_->GetWorldTransform());
			playerManager_->OnHCollision();
		}
	}
#pragma endregion
#pragma region レンジャーと敵キャラ投擲攻撃の当たり判定
	// 敵キャラ座標
	posA = enemyManager_->GetRockWorldPos();
	posB = renjuManager_->GetWorldPos();

	if (Math::IsAABBCollision(posA, {1.0f, 1.0f, 1.0f}, posB, {0.6f, 0.4f, 0.2f})) {
		if (enemyManager_->IsAttack()) {
			renjuManager_->OnCollision(enemyManager_->GetWorldTransform());
			playerManager_->OnRCollision();
		}
	}
#pragma endregion
#pragma region タンクと敵キャラ投擲攻撃の当たり判定
	// 敵キャラ座標
	posA = enemyManager_->GetRockWorldPos();
	posB = tankManager_->GetWorldPos();

	if (Math::IsAABBCollision(posA, {1.0f, 1.0f, 1.0f}, posB, {0.6f, 0.4f, 0.2f})) {
		if (enemyManager_->IsAttack()) {
			tankManager_->OnCollision(enemyManager_->GetWorldTransform());
			playerManager_->OnTCollision();
		}
	}
#pragma endregion


/*------------------
  仲間同士の当たり判定	
-------------------*/

#pragma region ヒーラーとレンジャーの当たり判定
	posA = healerManager_->GetWorldPos();
	posB = renjuManager_->GetWorldPos();
	if (Math::IsAABBCollision(posA, {0.6f, 0.4f, 0.2f}, posB, {0.6f, 0.4f, 0.2f})) {
		healerManager_->OnAllyCollision(renjuManager_->GetWorldTransform());
		renjuManager_->OnAllyCollision(healerManager_->GetWorldTransform());
	}

#pragma endregion
#pragma region タンクとレンジャーの当たり判定
	posA = tankManager_->GetWorldPos();
	posB = renjuManager_->GetWorldPos();
	if (Math::IsAABBCollision(posA, {0.6f, 0.4f, 0.2f}, posB, {0.6f, 0.4f, 0.2f})) {
		tankManager_->OnAllyCollision(renjuManager_->GetWorldTransform());
		renjuManager_->OnAllyCollision(tankManager_->GetWorldTransform());
	}

#pragma endregion
#pragma region ヒーラーとタンクの当たり判定
	posA = healerManager_->GetWorldPos();
	posB = tankManager_->GetWorldPos();
	if (Math::IsAABBCollision(posA, {0.6f, 0.4f, 0.2f}, posB, {0.6f, 0.4f, 0.2f})) {
		healerManager_->OnAllyCollision(tankManager_->GetWorldTransform());
		tankManager_->OnAllyCollision(healerManager_->GetWorldTransform());
	}

#pragma endregion
#pragma region プレイヤーとタンクの当たり判定
	posA = playerManager_->GetWorldPos();
	posB = tankManager_->GetWorldPos();
	if (Math::IsAABBCollision(posA, {0.6f, 0.4f, 0.2f}, posB, {0.4f, 0.6f, 0.2f})) {
		tankManager_->OnAllyCollision(playerManager_->GetWorldTransform());
	}

#pragma endregion
}