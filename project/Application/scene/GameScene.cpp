#include "GameScene.h"
#include "Framework/SceneManager.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>
#include <numbers>
#include <PostEffects/PostEffect.h>

GameScene::GameScene() {}

GameScene::~GameScene() {}
/*
GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}
*/

void GameScene::Initialize() {


	//衝突マネージャーの作成
	collisionManager_ = std::make_unique<CollisionManager>();

	viewProjection_.Initialize();
	viewProjection_.translation_ = { 0.0f, 0.0f, -10.0f };

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
	spriteBack_.reset(Sprite::Create("resources/Black.png"));
	spriteBack_->SetSize({ 1280.0f, 720.0f });

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
	if (Input::PushKey(DIK_P)) {
		SceneManager::GetInstance()->ChangeScene("ClearScene");
	}


	if (enemyManager_->IsClear() && isFadeOut_) {
		SceneManager::GetInstance()->ChangeScene("ClearScene");
	}

	if (playerManager_->IsOver() && isFadeOut_) {
		SceneManager::GetInstance()->ChangeScene("OverScene");
	}


	if (!isFadeIn_) {

	}

	playerManager_->Update();
	if (!enemyManager_->IsClear()) {
		healerManager_->Update();
		renjuManager_->Update();
		tankManager_->Update();
	}

	enemyManager_->Update();

	if (playerManager_->GetPlayer()->IsDash()) {
		PostEffect::GetInstance()->isRadialBlur(true);
	}
	else {
		PostEffect::GetInstance()->isRadialBlur(false);
	}

	// 追従カメラの更新
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();
	skydome_->Update();
	ground_->Update();
	//プレイヤーに追従
	healerManager_->followPlayer(playerManager_->GetPlayer()->GetWorldPosition());
	renjuManager_->followPlayer(playerManager_->GetPlayer()->GetWorldPosition());
	tankManager_->followPlayer(playerManager_->GetPlayer()->GetWorldPosition());
	//敵の座標の取得
	healerManager_->SetEnemypPos(enemyManager_->GetEnemy()->GetWorldPosition());
	renjuManager_->SetEnemypPos(enemyManager_->GetEnemy()->GetWorldPosition());
	tankManager_->SetEnemypPos(enemyManager_->GetEnemy()->GetWorldPosition());
	//プレイヤーと味方座標の取得
	if (!enemyManager_->IsAttack()) {
		enemyManager_->SetPlayerPos(playerManager_->GetPlayer()->GetWorldPosition());
		enemyManager_->SetHealerPos(healerManager_->GetHealer()->GetWorldPosition());
		enemyManager_->SetRenjuPos(renjuManager_->GetRenju()->GetWorldPosition());
		enemyManager_->SetTankPos(tankManager_->GetTank()->GetWorldPosition());
	}

	healerManager_->SetViewProjection(viewProjection_);
	renjuManager_->SetViewProjection(viewProjection_);
	tankManager_->SetViewProjection(viewProjection_);
	CheckAllCollision();


	playerManager_->GetPlayer()->SetLight(directionLight_);
	enemyManager_->GetEnemy()->SetLight(directionLight_);
	healerManager_->GetHealer()->SetLight(directionLight_);
	renjuManager_->GetRenju()->SetLight(directionLight_);
	tankManager_->GetTank()->SetLight(directionLight_);
	ground_->SetLight(directionLight_);
}


void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	//Model::LightDraw(color_, direction_, intensity_);

	
	//地面
	ground_->Draw(viewProjection_, false);
	//プレイヤー
	if (!playerManager_->GetPlayer()->IsDash()) {
		playerManager_->Draw(viewProjection_);
	}
	//敵
	enemyManager_->Draw(viewProjection_);
	// タンク
	tankManager_->Draw(viewProjection_);
	// ヒーラー
	healerManager_->Draw(viewProjection_);
	// レンジャー
	renjuManager_->Draw(viewProjection_);

	// 天球
	skydome_->Draw(viewProjection_, false);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();

	enemyManager_->DrawUI();
	playerManager_->DrawUI();

	Transform uv;
	uv.scale = { 0.0f, 0.0f, 0.0f };
	uv.rotate = { 0.0f, 0.0f, 0.0f };
	uv.translate = { 0.0f, 0.0f, 0.0f };

	spriteBack_->Draw(uv);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

void GameScene::RenderDirect() {
	if (playerManager_->GetPlayer()->IsDash()) {
		playerManager_->Draw(viewProjection_);
	}
	
}

void GameScene::Fade() {
	if (isFadeIn_) {
		if (alpha_ > 0.001f) {
			alpha_ -= 0.02f;
		}
		else {
			alpha_ = 0.0f;
			isFadeIn_ = false;
		}
	}

	if (playerManager_->IsOver() || enemyManager_->IsClear()) {
		if (alpha_ < 1) {
			alpha_ += 0.02f;
		}
		else {
			alpha_ = 1.0f;
			isFadeOut_ = true;
		}
	}

	if (playerManager_->IsOver()) {
		PostEffect::GetInstance()->isGrayscale(false);
	}

	spriteBack_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });
}

void GameScene::CheckAllCollision() {
	//コリジョン関係
	collisionManager_->ClearColliderList();
	collisionManager_->SetColliderList(playerManager_->GetPlayer());
	collisionManager_->SetColliderList(enemyManager_->GetEnemy());
	collisionManager_->SetColliderList(healerManager_->GetHealer());
	collisionManager_->SetColliderList(renjuManager_->GetRenju());
	collisionManager_->SetColliderList(tankManager_->GetTank());
	collisionManager_->CheckAllCollisions();

	//敵の攻撃フラグの受け取り
	playerManager_->GetPlayer()->SetEnemyAttack(enemyManager_->IsAttack());
	healerManager_->GetHealer()->SetEnemyAttack(enemyManager_->IsAttack());
	renjuManager_->GetRenju()->SetEnemyAttack(enemyManager_->IsAttack());
	tankManager_->GetTank()->SetEnemyAttack(enemyManager_->IsAttack());

	//死亡フラグの受け取り
	playerManager_->GetPlayer()->SetTankDead(tankManager_->GetTank()->IsDead());
	playerManager_->GetPlayer()->SetRenjuDead(renjuManager_->GetRenju()->IsDead());
	playerManager_->GetPlayer()->SetHealerDead(healerManager_->GetHealer()->IsDead());


	// 判定対象AとBの座標
	Vector3 posA, posB;

	const std::list<RenjuBullet*>& renjuBullets = renjuManager_->GetBullets();

#pragma region 敵とレンジャーの弾の当たり判定
	// 敵キャラ座標
	posA = { enemyManager_->GetEnemy()->GetWorldPosition().x,enemyManager_->GetEnemy()->GetWorldPosition().y + 6.0f,enemyManager_->GetEnemy()->GetWorldPosition().z };
	//レンジャー弾
	for (RenjuBullet* bullet : renjuBullets) {
		posB = bullet->GetWorldPosition();

		if (Math::IsAABBCollision(posA, { 2.0f, 6.0f, 1.0f }, posB, { 0.3f, 0.3f, 0.3f })) {
			bullet->OnCollision();
			enemyManager_->OnCollision();
			renjuManager_->SetParticlePos(posB);
		}

	}



#pragma endregion

#pragma region 敵キャラとタンクの当たり判定

	// 敵キャラ座標
	posA = { enemyManager_->GetEnemy()->GetWorldPosition().x, enemyManager_->GetEnemy()->GetWorldPosition().y + 6.0f,enemyManager_->GetEnemy()->GetWorldPosition().z };
	posB = tankManager_->GetTank()->GetWorldPosition();

	if (Math::IsAABBCollision(posA, { 2.0f, 6.0f, 1.0f }, posB, { 1.0f, 1.0f, 1.0f })) {
		if (tankManager_->GetAttack()) {
			enemyManager_->OnCollision();
			tankManager_->SetParticlePos(posB);
		}

	}

#pragma endregion

#pragma region 敵キャラとヒーラーの攻撃当たり判定
	// 敵キャラ座標
	posA = { enemyManager_->GetEnemy()->GetWorldPosition().x,enemyManager_->GetEnemy()->GetWorldPosition().y + 6.0f,enemyManager_->GetEnemy()->GetWorldPosition().z };
	posB = healerManager_->GetCanePos();

	if (Math::IsAABBCollision(posA, { 2.0f, 6.0f, 1.0f }, posB, { 0.27f, 0.27f, 1.0f })) {
		if (healerManager_->IsAttack()) {
			enemyManager_->OnCollision();
			healerManager_->SetParticlePos(posB);
		}

	}
#pragma endregion

#pragma region 敵キャラと自キャラの攻撃当たり判定
	// 敵キャラ座標
	posA = { enemyManager_->GetEnemy()->GetWorldPosition().x,enemyManager_->GetEnemy()->GetWorldPosition().y + 6.0f,enemyManager_->GetEnemy()->GetWorldPosition().z };
	posB = playerManager_->katanaPos();


	if (Math::IsAABBCollision(posA, { 2.0f, 6.0f, 1.5f }, posB, { 0.1f, 1.0f, 0.1f })) {
		if (playerManager_->IsAttack()) {
			enemyManager_->OnCollision();
			playerManager_->SetParticlePos(posB);
		}

	}
#pragma endregion

	/*------------------
	  敵の攻撃の当たり判定
	-------------------*/


#pragma region 自キャラと敵キャラ投擲攻撃の当たり判定
	// 敵キャラ座標
	posA = enemyManager_->GetRockWorldPos();
	posB = playerManager_->GetPlayer()->GetWorldPosition();

	if (Math::IsAABBCollision(posA, { 1.0f, 1.0f, 1.0f }, posB, { 0.6f, 0.4f, 0.2f })) {
		if (enemyManager_->IsAttack()) {
			playerManager_->GetPlayer()->OnCollision(enemyManager_->GetWorldTransform());
		}
	}
#pragma endregion
#pragma region ヒーラーと敵キャラ投擲攻撃の当たり判定
	// 敵キャラ座標
	posA = enemyManager_->GetRockWorldPos();
	posB = healerManager_->GetHealer()->GetWorldPosition();

	if (Math::IsAABBCollision(posA, { 1.0f,1.0f, 1.0f }, posB, { 0.6f, 0.4f, 0.2f })) {
		if (enemyManager_->IsAttack()) {
			healerManager_->GetHealer()->OnCollision(enemyManager_->GetWorldTransform());
			playerManager_->OnHCollision();
		}
	}
#pragma endregion
#pragma region レンジャーと敵キャラ投擲攻撃の当たり判定
	// 敵キャラ座標
	posA = enemyManager_->GetRockWorldPos();
	posB = renjuManager_->GetRenju()->GetWorldPosition();

	if (Math::IsAABBCollision(posA, { 1.0f, 1.0f, 1.0f }, posB, { 0.6f, 0.4f, 0.2f })) {
		if (enemyManager_->IsAttack()) {
			renjuManager_->OnCollision(enemyManager_->GetWorldTransform());
			playerManager_->OnRCollision();
		}
	}
#pragma endregion
#pragma region タンクと敵キャラ投擲攻撃の当たり判定
	// 敵キャラ座標
	posA = enemyManager_->GetRockWorldPos();
	posB = tankManager_->GetTank()->GetWorldPosition();

	if (Math::IsAABBCollision(posA, { 1.0f, 1.0f, 1.0f }, posB, { 0.6f, 0.4f, 0.2f })) {
		if (enemyManager_->IsAttack()) {
			tankManager_->OnCollision(enemyManager_->GetWorldTransform());
			playerManager_->OnTCollision();
		}
	}
#pragma endregion



}