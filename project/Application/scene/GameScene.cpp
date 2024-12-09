#include "GameScene.h"
#include "Framework/SceneManager.h"
#include "TextureManeger.h"
#include <cassert>
#include <imgui.h>
#include <numbers>
#include <PostEffects/PostEffect.h>



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
	loader_.reset(ModelLoader::Create("resources/JsonFile/loader.json"));

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

	//コマンド
	command_ = std::make_unique<Command>();
	command_->Initialize();

	//フェード
	isFadeIn_ = true;
	isFadeOut_ = false;

	//バトルのフラグ
	battle_ = false;

	//ラジアルブラー
	radialBlur_.isEnble = false;
	radialBlur_.blurWidth = 0.01f;
	radialBlur_.rotation = 0.1f;
	

	characters = {healerManager_->GetHealer(), renjuManager_->GetRenju(),tankManager_->GetTank()};

	PostEffect::GetInstance()->isBloom(true);
}

void GameScene::Update() {

	//シーン遷移
	Fade();
	BattleBegin();
	
	//当たり判定
	CheckAllCollision();

	//敵の更新
	enemyManager_->Update();
	if (playerManager_->GetPlayer()->IsDead()) {
		enemyManager_->Stop();
	}

	healerManager_->GetHealer()->SetBarrier(tankManager_->GetTank()->GetBarrier());
	renjuManager_->GetRenju()->SetBarrier(tankManager_->GetTank()->GetBarrier());

	//各キャラの情報の取得
	playerManager_->GetPlayer()->SetBarrierPos(tankManager_->GetTank()->GetBarrierWorldPos());

	//敵の情報取得
	playerManager_->GetPlayer()->SetEnemy(enemyManager_->GetEnemy());
	
	//playerの更新
	playerManager_->GetPlayer()->SetEnemyLength(enemyManager_->GetWorldTransform().translate);
	if (radialBlur_.blurWidth == 0.01f) {
		playerManager_->Update();
	}
	

	//コマンドの更新
	if (!playerManager_->GetPlayer()->IsOuto() && battle_) {
		command_->SetBehavior(playerManager_->GetPlayer()->IsRoot());
		command_->Update();
	}
	
	for (int i = 0; i < kAttackMax; ++i) {
		playerManager_->GetPlayer()->SetAttack(command_->GetAttack(i), i);
	}

	for (auto* character : characters) {
		//敵情報の受け取り
		character->SetEnemy(enemyManager_->GetEnemy());
		//作戦の受け取り
		character->SetOperation(command_->GetOperatin());
		//プレイヤーに追従
		character->SetPlayerPos(playerManager_->GetPlayer()->GetWorldPosition());
		//倒された時のディゾルブ
		if (playerManager_->GetPlayer()->GetHp() <= 0) {
			character->Dissolve();
		}
		character->SetBarrierPos(tankManager_->GetTank()->GetBarrierWorldPos());
		if (enemyManager_->GetEnemy()->IsSpecial()) {
			for (EnemyHenchman* enemy : enemyManager_->GetEnemy()->GetEnemys()) {
				character->SetHenchman(enemy);
			}
		}
	}

	healerManager_->GetHealer()->SetBarrierThreshold(tankManager_->GetTank()->GetBarrierThreshold());
	renjuManager_->GetRenju()->SetBarrierThreshold(tankManager_->GetTank()->GetBarrierThreshold());


	//魔法陣に使う座標の受け取り
	healerManager_->GetHealer()->SetPos(renjuManager_->GetRenju()->GetWorldPosition(), tankManager_->GetTank()->GetWorldPosition());
	renjuManager_->GetRenju()->SetTankPos(tankManager_->GetTank()->GetWorldPosition());
	//回復
	if (healerManager_->GetHealer()->GetHeal()) {
		//全体回復
		if (healerManager_->GetHealer()->GetAllHeal()) {
			playerManager_->GetPlayer()->SetHeal(healerManager_->GetHealer()->GetHealAmount());
			if (!healerManager_->GetHealer()->IsDead()) {
				healerManager_->GetHealer()->SetHeal(healerManager_->GetHealer()->GetHealAmount());
			}
			if (!renjuManager_->GetRenju()->IsDead()) {
				renjuManager_->GetRenju()->SetHeal(healerManager_->GetHealer()->GetHealAmount());
			}
			if (!tankManager_->GetTank()->IsDead()) {
				tankManager_->GetTank()->SetHeal(healerManager_->GetHealer()->GetHealAmount());
			}

		}
		//個人回復
		if (healerManager_->GetHealer()->GetOneHeal()) {
			if (healerManager_->GetHealer()->GetPlayerHp() <= 30 && playerManager_->GetPlayer()->GetHp() >= 1) {
				playerManager_->GetPlayer()->SetHeal(healerManager_->GetHealer()->GetHealAmount());
			}
			else if (healerManager_->GetHealer()->GetHp() <= 30 && !healerManager_->GetHealer()->IsDead()) {
				healerManager_->GetHealer()->SetHeal(healerManager_->GetHealer()->GetHealAmount());
			}
			else if (healerManager_->GetHealer()->GetRenjuHp() <= 30 && !renjuManager_->GetRenju()->IsDead()) {
				renjuManager_->GetRenju()->SetHeal(healerManager_->GetHealer()->GetHealAmount());
			}
			else if (healerManager_->GetHealer()->GetTankHp() <= 30 && !tankManager_->GetTank()->IsDead()) {
				tankManager_->GetTank()->SetHeal(healerManager_->GetHealer()->GetHealAmount());
			}

		}
	}

	//hp情報の取得
	if (healerManager_->GetHealer()->GetState() != CharacterState::Unique) {
		healerManager_->GetHealer()->SetHp({ playerManager_->GetPlayer()->GetHp(),renjuManager_->GetRenju()->GetHp(),tankManager_->GetTank()->GetHp() });
	}
	

	//各キャラの更新
	if (!enemyManager_->IsClear()) {
		tankManager_->Update();
		renjuManager_->Update();
		healerManager_->Update();
		
	}

	//生存しているかどうか
	enemyManager_->GetEnemy()->SetIsDeadPlayer(playerManager_->GetPlayer()->GetIsDead());
	enemyManager_->GetEnemy()->SetIsDeadHealer(healerManager_->GetHealer()->IsDead());
	enemyManager_->GetEnemy()->SetIsDeadRenju(renjuManager_->GetRenju()->IsDead());
	enemyManager_->GetEnemy()->SetIsDeadTank(tankManager_->GetTank()->IsDead());

	//プレイヤーと味方座標の取得
	enemyManager_->SetPlayerPos(playerManager_->GetPlayer()->GetWorldPosition());
	if (!enemyManager_->GetEnemy()->IsBehaberAttack()) {
		enemyManager_->SetHealerPos(healerManager_->GetHealer()->GetWorldPosition());
		enemyManager_->SetRenjuPos(renjuManager_->GetRenju()->GetWorldPosition());
		enemyManager_->SetTankPos(tankManager_->GetTank()->GetWorldPosition());
	}
	//必殺技の時tankだけposを受け取る
	if (enemyManager_->GetEnemy()->GetBehavior() == Behavior::kAttack && enemyManager_->GetEnemy()->GetBehaviorAttack() == BehaviorAttack::kBreath) {
		enemyManager_->SetTankPos(tankManager_->GetTank()->GetWorldPosition());
	}
	//ブレスパーティクルのフィールド用
	enemyManager_->GetEnemy()->SetTankRotation(tankManager_->GetTank()->GetWorldTransform().rotate);
	enemyManager_->GetEnemy()->SethmansRenjuPos(renjuManager_->GetRenju()->GetWorldPosition());

	//レンジャーを守るためのpos
	tankManager_->GetTank()->SetPos(renjuManager_->GetRenju()->GetWorldPosition());

	//攻撃の受け取り
	if (healerManager_->IsAttack()) {enemyManager_->OnHealerCollision();}
	if (tankManager_->IsAttack()) {enemyManager_->OnTankCollision();}
	if (playerManager_->IsAttack()) {enemyManager_->OnCollision();}
	if (renjuManager_->GetRenju()->GetHitBullet()) {
		enemyManager_->OnRenjuCollision(renjuManager_->GetRenju()->GetSkill());
		enemyManager_->GetEnemy()->SetRenjuSpecial(renjuManager_->GetRenju()->GetSpecial());
	}
	skydome_->Update();
	loader_->Update();

	// 追従カメラの更新
	if ((!cameraDirection_||battle_) && radialBlur_.blurWidth == 0.01f) {
		followCamera_->Update();
	}
	viewProjection_.matView = followCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
	viewProjection_.TransferMatrix();

	//viewの設定
	enemyManager_->SetCamera(viewProjection_);
	playerManager_->GetPlayer()->SetCamera(viewProjection_);
	//ライティングの設定
	playerManager_->GetPlayer()->SetLight(directionLight_);
	enemyManager_->GetEnemy()->SetLight(directionLight_);

	//view・ライティングの設定
	for (auto* character : characters) {
		character->SetViewProjection(viewProjection_);
		character->SetLight(directionLight_);
	}
	
	loader_->SetLight(directionLight_);

}

void GameScene::Draw() {
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	//Model::LightDraw(color_, direction_, intensity_);

	// 天球
	skydome_->Draw(viewProjection_, false);
	//地面
	loader_->Draw(viewProjection_, true);
	
	//敵
	enemyManager_->Draw(viewProjection_);
	// タンク
	tankManager_->Draw(viewProjection_);
	// ヒーラー
	healerManager_->Draw(viewProjection_);
	// レンジャー
	renjuManager_->Draw(viewProjection_);
	
	//ポストエフェクトをかける
	if (!playerManager_->GetPlayer()->IsDash()/*|| (playerManager_->GetPlayer()->GameStart() && !cameraDirection_) || (tankManager_->GetTank()->GetBarrier() || tankManager_->GetTank()->GetBarrierThreshold() < 0.5f)*/ ) {
		//プレイヤー
		playerManager_->Draw(viewProjection_);
	}

	//バリアの描画
	enemyManager_->GetEnemy()->BarrierDraw(viewProjection_);
	tankManager_->GetTank()->BarrierDraw(viewProjection_);

	//コマンド
	if (battle_) {
		command_->Draw(viewProjection_);
	}
	
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw();
	if (playerManager_->GetPlayer()->IsDash() || (playerManager_->GetPlayer()->GameStart() && !cameraDirection_)) {
		enemyManager_->DrawUI();
		playerManager_->DrawUI();
		for (auto* character : characters) {
			character->DrawUI();
		}

		//コマンド
		if (battle_) {
			command_->DrawUI();
		}
	}

	

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

void GameScene::RenderDirect() {
	
	//ダッシュ中はポストエフェクトをかけない
	if (playerManager_->GetPlayer()->IsDash()) {
		playerManager_->Draw(viewProjection_);
	}

	if (!playerManager_->GetPlayer()->IsDash() && (playerManager_->GetPlayer()->GameStart() && cameraDirection_)) {
		//if (!tankManager_->GetTank()->GetBarrier()) {
		//	//敵
		//	enemyManager_->Draw(viewProjection_);
		//	// タンク
		//	tankManager_->Draw(viewProjection_);
		//	// ヒーラー
		//	healerManager_->Draw(viewProjection_);
		//	// レンジャー
		//	renjuManager_->Draw(viewProjection_);
		//	//プレイヤー
		//	playerManager_->Draw(viewProjection_);
		//}
		
		enemyManager_->DrawUI();
		playerManager_->DrawUI();
		for (auto* character : characters) {
			character->DrawUI();
		}

		//コマンド
		if (battle_) {
			command_->DrawUI();
		}
	}
	
	spriteBack_->Draw();


	//デプスのないオブジェクト
	playerManager_->GetPlayer()->NoDepthDraw(viewProjection_);
	healerManager_->GetHealer()->NoDepthDraw(viewProjection_);
	renjuManager_->GetRenju()->NoDepthDraw(viewProjection_);
	tankManager_->GetTank()->NoDepthDraw(viewProjection_);
	enemyManager_->NoDepthDraw(viewProjection_);

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

	if (enemyManager_->IsClear() && isFadeOut_) {
		SceneManager::GetInstance()->ChangeScene("ClearScene");
	}

	if (playerManager_->IsOver() && isFadeOut_) {
		SceneManager::GetInstance()->ChangeScene("OverScene");
	}

}

void GameScene::BattleBegin(){
	PostEffect* const posteffect = PostEffect::GetInstance();
	//ラジアルブラーの演出
	if (playerManager_->GetPlayer()->GameStart() && !cameraDirection_ && radialBlur_.blurWidth < 0.9f) {
		posteffect->isRadialBlur(true);

		radialBlur_.blurWidth = Math::LerpShortAngle(radialBlur_.blurWidth,1.0f,0.1f);
		cameraDirectionTime_ = 10;

		healerManager_->GetHealer()->SetGameStart(true);
		renjuManager_->GetRenju()->SetGameStart(true);
		tankManager_->GetTank()->SetGameStart(true);
	}

	//カメラの演出に移行
	if (radialBlur_.blurWidth >= 0.9f && !cameraDirection_) {
		--cameraDirectionTime_;
		if (cameraDirectionTime_ <= 0) {
			posteffect->isRadialBlur(false);
			radialBlur_.blurWidth = 0.01f;
			radialBlur_.rotation = 0.f;
			cameraDirection_ = true;
			//位置の初期化
			playerManager_->GetPlayer()->InitPos();
			healerManager_->GetHealer()->InitPos(6.f);
			renjuManager_->GetRenju()->InitPos(-3.f);
			tankManager_->GetTank()->InitPos(0.f);
			//カメラの初期化
			followCamera_->InitAngle();
			followCamera_->SetinterTargetPos({ 3.0f,0.0f,-35.0f });
		}
	}

	posteffect->RadialBlurWidth(radialBlur_.blurWidth);
	posteffect->RadialBlurRotation(radialBlur_.rotation);

	//カメラが一定の位置に来たらバトル開始
	if (cameraDirection_ && !battle_) {
		followCamera_->CameraDirection();
		if (!followCamera_->GetMoveToEnemy()) {
			enemyManager_->GetEnemy()->SetAnimationNumber(threat, 25.f,false);
		}

		//カメラがプレイヤーのとこまで来たらスタート
		if (followCamera_->GetViewProjection().translation_.z <= -49.9f) {
			battle_ = true;
			command_->SetTaskType(TaskType::kInitial);
			command_->SetOperatin(false);
		}
	}
	
	//バトル開始のフラグ受け取り
	playerManager_->GetPlayer()->SetBattleStart(battle_);
	enemyManager_->GetEnemy()->SetBattleStart(battle_);
	healerManager_->GetHealer()->SetBattleStart(battle_);
	renjuManager_->GetRenju()->SetBattleStart(battle_);
	tankManager_->GetTank()->SetBattleStart(battle_);
}

void GameScene::CheckAllCollision() {
	//コリジョン関係
	collisionManager_->ClearColliderList();
	collisionManager_->SetColliderList(playerManager_->GetPlayer());
	collisionManager_->SetColliderList(enemyManager_->GetEnemy());
	if (enemyManager_->GetEnemy()->isAttack()) {
		collisionManager_->SetColliderList(enemyManager_->GetEnemy()->GetRockCollider());

		for (int i = 0; i < 15; ++i) {
			collisionManager_->SetColliderList(enemyManager_->GetEnemy()->GetCollider(i));
		}
	}
	
	for (auto* character : characters) {
		collisionManager_->SetColliderList(character);
	}
	/*collisionManager_->SetColliderList(healerManager_->GetHealer());
	collisionManager_->SetColliderList(renjuManager_->GetRenju());	
	collisionManager_->SetColliderList(tankManager_->GetTank());*/

	for (RenjuBullet* bullet : renjuManager_->GetRenju()->GetBullets()) {
		collisionManager_->SetColliderList(bullet);
	}
	
	for (EnemyHenchman* enemy : enemyManager_->GetEnemy()->GetEnemys()) {
		collisionManager_->SetColliderList(enemy);
	}

	for (int i = 0; i < loader_->GetColliderSize(); ++i) {
		collisionManager_->SetColliderList(loader_->GetCollider(i));
	}
	collisionManager_->CheckAllCollisions();

	//死亡フラグの受け取り
	playerManager_->GetPlayer()->SetTankDead(tankManager_->GetTank()->IsDead());
	playerManager_->GetPlayer()->SetRenjuDead(renjuManager_->GetRenju()->IsDead());
	playerManager_->GetPlayer()->SetHealerDead(healerManager_->GetHealer()->IsDead());

}