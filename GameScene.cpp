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
	viewProjection_.Initialize();
	viewProjection_.translation_ = {0.0f, 0.0f, -5.0f};

	modelPlayer_.reset(Model::CreateModelFromObj("resources/cube.obj", "resources/ball.png"));
	player_ = std::make_unique<Player>();
	std::vector<Model*> playerModels = {modelPlayer_.get()};
	player_->Initialize(playerModels);


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

	// レールカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());

	// 自キャラの生成と初期化処理
	player_->SetViewProjection(followCamera_.get()->GetViewProjection());
}

void GameScene::Update() {
	// 追従カメラの更新
	followCamera_->Update();
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;

	viewProjection_.TransferMatrix();

	player_->Update();
	skydome_->Update();
	ground_->Update();
	
   
	CheckAllCollision();

	ImGui::Begin("Player");
	
	ImGui::Text(" %d", c);
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

	//プレーヤー
	player_->Draw(viewProjection_);
	

	
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



void GameScene::CheckAllCollision() {
    // 判定対象AとBの座標
	Vector3 posA, posB[2];
	
#pragma region 自キャラと地面の当たり判定
    // 自キャラ座標
	posA = player_->GetWorldPosition();
  
    // 自キャラと敵弾全ての当たり判定
    for (int i = 0; i < 2; i++) {
        // 敵弾の座標
		posB[i] = ground_->GetWorldPosition(i);
		
		if (Math::IsAABBCollision(
		        posA, player_->GetWorldTransform().scale, posB[i],ground_->GetWorldTransform(i).scale)) {
            // 自キャラの衝突時コールバックを呼び出す
			if (posA.y >= -5.0f) {
				player_->OnCollision();
			}
          
			c = true;
			break;
        }
        else {
			c = false;
            player_->OutCollision();
			
        }
    }

#pragma endregion
}
