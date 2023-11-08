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
	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -5.0f};

	//プレイヤー
	modelPlayer_.reset(Model::CreateModelFromObj("resources/cube.obj", "resources/ball.png"));
	modelHammer_.reset(Model::CreateModelFromObj("resources/hammer/hammer.obj", "resources/hammer/hammer.png"));
	player_ = std::make_unique<Player>();
	std::vector<Model*> playerModels = {modelPlayer_.get(), modelHammer_.get()};
	player_->Initialize(playerModels);

	//敵
	modelEnemy_.reset(Model::CreateModelFromObj("resources/cube.obj", "resources/enemy.png"));
	enemy_ = std::make_unique<Enemy>();
	std::vector<Model*> enemyModels = {modelEnemy_.get()};
	enemy_->Initialize(enemyModels);

	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(Model::CreateModelFromObj("resources/skydome.obj", "resources/skydome/sky.png"));
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

	// レールカメラ
	//followCamera_ = std::make_unique<FollowCamera>();
	//followCamera_->Initialize();
	//// 自キャラのワールドトランスフォームを追従カメラにセット
	//followCamera_->SetTarget(&player_->GetWorldTransform());

	//// 自キャラの生成と初期化処理
	//player_->SetViewProjection(&followCamera_->GetViewProjection());

	target_ = &player_->GetWorldTransform();

	// 自キャラの生成と初期化処理
	player_->SetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	
	floor_->Update();
	
	player_->Update();
	enemy_->Update();
	ground_->Update();
	skydome_->Update();
	
	CheckAllCollision();
	
	// 追従カメラの更新
	/*followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();*/


	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ジョイスティックの状態取得
	if (KeyInput::GetInstance()->GetJoystickState(0, joyState)) {
		// 回転速度
		float kCharacterSpeed = 0.05f;

		destinationAngleY_ += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kCharacterSpeed;
		destinationAngleX_ -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * kCharacterSpeed / 5;
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) {
			destinationAngleY_ = target_->rotate.y;
			destinationAngleX_ = target_->rotate.x;
		}
	}

	viewProjection_.rotation_.y =
	    Math::LerpShortAngle(viewProjection_.rotation_.y, destinationAngleY_, 0.1f);
	viewProjection_.rotation_.x =
	    Math::LerpShortAngle(viewProjection_.rotation_.x, destinationAngleX_, 0.1f);

	// 追従対象がいれば
	if (target_) {
		// 追従座標の補間
		interTarget_ = Math::Lerp(interTarget_, target_->translate, delayAmount_);
	}
	// 追跡対象からカメラまでのオフセット
	Vector3 offset = calculateOffset();

	// 座標をコピーしてオフセット分ずらす
	viewProjection_.translation_ = Math::Add(interTarget_, offset);

	// ビュー行列の更新
	viewProjection_.UpdateMatrix();
	viewProjection_.TransferMatrix();

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
	Model::PreDraw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	// 天球
	skydome_->Draw(viewProjection_);
	// 地面
	ground_->Draw(viewProjection_);
	//床
	floor_->Draw(viewProjection_);
	//プレーヤー
	player_->Draw(viewProjection_);
	//敵
	enemy_->Draw(viewProjection_);
	
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}


Vector3 GameScene::calculateOffset() const {
	// 追従対象からのオフセット
	Vector3 offset = {0.0f, 4.0f, -15.0f};

	Matrix4x4 rotateMatrix = Math::Multiply(
	    Math::MakeRotateXMatrix(viewProjection_.rotation_.x),
	    Math::Multiply(
	        Math::MakeRotateYMatrix(viewProjection_.rotation_.y),
	        Math::MakeRotateZMatrix(viewProjection_.rotation_.z)));

	offset = Math::TransformNormal(offset, rotateMatrix);

	return offset;
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
		        posA, player_->GetWorldTransform().scale, posB[i],ground_->GetWorldTransform(i).scale)) {
            // 自キャラの衝突時コールバックを呼び出す
			if (posA.y >= -5.0f) {
				player_->OnCollision();
			}
          
			collision1_ = true;
			break;
        }
        else {
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
	        posA, player_->GetWorldTransform().scale, posB[0],
	        floor_->GetWorldTransform().scale)) {
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
	//自キャラ座標
	posA = player_->GetWorldPosition();

	// 移動床の座標
	posB[0] = enemy_->GetWorldPosition();

	if (Math::IsAABBCollision(
	        posA, player_->GetWorldTransform().scale, posB[0], enemy_->GetWorldTransform().scale)) {
		// 自キャラの衝突時コールバックを呼び出す
		if (enemy_->IsDead()) {
			player_->SetPosition({0.0f, 0.0f, 0.0f});
		}
		
		
	}

#pragma endregion

#pragma region 自の攻撃と敵
	// 自キャラ座標
	posA = player_->GetHammerWorldPos();

	// 移動床の座標
	posB[0] = enemy_->GetWorldPosition();

	// 距離
	float distance = (posB[0].x - posA.x) * (posB[0].x - posA.x) +
	           (posB[0].y - posA.y) * (posB[0].y - posA.y) +
	           (posB[0].z - posA.z) * (posB[0].z - posA.z);
	float R1 = 1.0f;
	float R2 = 1.0f;
	if (distance <= (R1 + R2) * (R1 + R2)){
		// 敵キャラの衝突時コールバックを呼び出す
		if (player_->IsAttack()) {
			enemy_->OnCollision();
		}
		
		
	}

#pragma endregion
}
