#include "GameScene.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>


GameScene::GameScene() {}

GameScene::~GameScene() {}

GameScene* GameScene::GetInstance() {
	static GameScene instance;
	return &instance;
}

void GameScene::Initialize() {
	worldTransform_.Initialize();
	worldTransformParticle_.Initialize();
	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -5.0f};

	worldTransformParticle_.scale = {0.5f, 0.5f, 0.5f};

	// プレイヤー
	modelPlayer_.reset(
	    Model::CreateModelFromObj("resources/player/float_Head.obj", "resources/player/tex.png"));
	modelHammer_.reset(
	    Model::CreateModelFromObj("resources/hammer/hammer.obj", "resources/hammer/hammer.png"));
	player_ = std::make_unique<Player>();
	std::vector<Model*> playerModels = {modelPlayer_.get(), modelHammer_.get()};
	player_->Initialize(playerModels);

	// 敵
	for (int i = 0; i < 5; i++) {
		std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
		newEnemy->Initialize(enemyPos[i]);
		enemies_.push_back(std::move(newEnemy));

	}

	lockOnMark_.reset(Sprite::Create("resources/Reticle.png"));
	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(
	    Model::CreateModelFromObj("resources/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());

	// 地面
	ground_ = std::make_unique<Ground>();
	// 3Dモデルの生成
	modelGround_.reset(Model::CreateModelFromObj("resources/cube.obj", "resources/ground.png"));
	ground_->Initialize(modelGround_.get());

	// 床
	floor_ = std::make_unique<MoveFloor>();
	// 3Dモデルの生成
	modelFloor_.reset(Model::CreateModelFromObj("resources/cube.obj", "resources/floor.png"));
	floor_->Initialize(modelFloor_.get());


	emitter_.count = 20;
	emitter_.frequency = 0.5f;
	emitter_.frequencyTime = 0.0f;
	emitter_.transform = {
	    {0.5f, 0.5f, 0.5f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
	};
	//パーティクル
	particle_.reset(Particle::Create("resources/particle/circle.png", emitter_));
	particle_->SetSpeed(10.0f);

	// ロックオン
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	// レールカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());
	followCamera_->SetLockOn(lockOn_.get());
	// 自キャラの生成と初期化処理
	player_->SetViewProjection(&followCamera_->GetViewProjection());
	player_->SetLockOn(lockOn_.get());

	
}

void GameScene::Update() {

	floor_->Update();

	player_->Update();
	if (player_->IsMove()){
		//particle_->SetTranslate(player_->GetWorldPosition());
		//particle_->Update();
	} else {
		//particle_->StopParticles();
	}
	particle_->SetTranslate({0.0f,0.0f,5.0f});
	particle_->Update();

	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Update();	
		enemy->SetFinalAttack(player_->GetFinalAttack());
	}
	
	ground_->Update();
	skydome_->Update();

	CheckAllCollision();

	// 追従カメラの更新
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		if (!enemy->IsDead()) {
			//lockOn_->Update(enemies_, viewProjection_);
		}
	}
	lockOn_->Update(enemies_, viewProjection_);

	
	//worldTransformParticle_.UpdateMatrix();


	ImGui::Begin("scene");
	ImGui::DragFloat4("translation", &worldTransform_.translate.x, 0.01f);
	ImGui::Text(
	    " X%f Y%f Z%f", viewProjection_.matView.m[1][0], viewProjection_.matView.m[1][1],
	    viewProjection_.matView.m[1][2]);
	ImGui::Text(
	    " X%f Y%f Z%f", viewProjection_.matView.m[2][0], viewProjection_.matView.m[2][1],
	    viewProjection_.matView.m[2][2]);
	ImGui::Text(
	    " X%f Y%f Z%f", viewProjection_.matView.m[3][0], viewProjection_.matView.m[3][1],
	    viewProjection_.matView.m[3][2]);
	ImGui::Text(" collision1_%d", collision1_);
	ImGui::Text(" collision2_%d", collision2_);
	ImGui::End();
}

void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::LightDraw(lightColor_, direction_, intensity_);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 天球
	skydome_->Draw(viewProjection_,false);
	// 地面
	ground_->Draw(viewProjection_);
	// 床
	floor_->Draw(viewProjection_);
	// プレーヤー
	player_->Draw(viewProjection_,true);
	// 敵
	for (std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw(viewProjection_,true);
	}

	// パーティクル
	particle_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();

	lockOn_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

void GameScene::CheckAllCollision() {
	// 判定対象AとBの座標
	Vector3 posA, posB[2];

#pragma region 自キャラと地面の当たり判定
	// 自キャラ座標
	posA = player_->GetWorldPosition();

	// 自キャラと地面全ての当たり判定
	for (int i = 0; i < 2; i++) {
		// 地面の座標
		posB[i] = ground_->GetWorldPosition(i);

		if (Math::IsAABBCollision(
		        posA, player_->GetWorldTransform().scale, posB[i],
		        ground_->GetWorldTransform(i).scale)) {
			// 自キャラの衝突時コールバックを呼び出す
			if (posA.y >= -5.0f) {
				player_->OnCollision();
			}

			collision1_ = true;
			break;
		} else {
			collision1_ = false;
			if (!collision2_) {
				player_->OutCollision();
			}
		}
	}

#pragma endregion

#pragma region 自キャラと移動床の当たり判定
	// 自キャラ座標
	posA = player_->GetWorldPosition();

	// 移動床の座標
	posB[0] = floor_->GetWorldPosition();

	if (Math::IsAABBCollision(
	        posA, player_->GetWorldTransform().scale, posB[0], floor_->GetWorldTransform().scale)) {
		// 自キャラの衝突時コールバックを呼び出す
		player_->OnCollision();
		player_->OnCollisionFloor();

		player_->Relationship(floor_->GetWorldTransform());
		collision2_ = true;
	} else {
		player_->DeleteParent();
		collision2_ = false;
		if (!collision1_) {
			player_->OutCollisionFloor();
		}
	}

#pragma endregion

#pragma region 自キャラと敵
	for (auto& enemy : enemies_) {
		// 自キャラ座標
		posA = player_->GetWorldPosition();

		// 敵の座標
		posB[0] = enemy->GetWorldPosition();

		if (Math::IsAABBCollision(
		        posA, player_->GetWorldTransform().scale, posB[0],
		        enemy->GetWorldTransform().scale)) {
			// 自キャラの衝突時コールバックを呼び出す
			if (!enemy->IsDead()) {
				player_->SetPosition({0.0f, 0.0f, 0.0f});
			}
		}
	}

#pragma endregion

#pragma region 自キャラの攻撃と敵
	for (auto& enemy : enemies_) {
		// 自キャラ座標
		posA = player_->GetHammerWorldPos();

		// 移動床の座標
		posB[0] = enemy->GetWorldPosition();

		// 距離
		float distance = (posB[0].x - posA.x) * (posB[0].x - posA.x) +
		                 (posB[0].y - posA.y) * (posB[0].y - posA.y) +
		                 (posB[0].z - posA.z) * (posB[0].z - posA.z);
		float R1 = 1.5f;
		float R2 = 1.0f;
		if (distance <= (R1 + R2) * (R1 + R2)) {
			// 敵キャラの衝突時コールバックを呼び出す
			if (player_->IsAttack()) {
				enemy->OnCollision(player_->GetWorldTransform());
			}
		}
	}
#pragma endregion
}