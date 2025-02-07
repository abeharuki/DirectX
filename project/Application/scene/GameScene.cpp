#include "GameScene.h"
#include "Framework/SceneManager.h"


void GameScene::Initialize() {

	//衝突マネージャーの作成
	collisionManager_ = std::make_unique<CollisionManager>();

	viewProjection_.Initialize();
	
	

	// 天球
	skydome_ = std::make_unique<Skydome>();
	// 3Dモデルの生成
	modelSkydome_.reset(Model::CreateModelFromObj("resources/skydome/skydome.obj", "resources/skydome/sky.png"));
	skydome_->Initialize(modelSkydome_.get());

	// 地面
	loader_.reset(ModelLoader::Create("resources/JsonFile/loader.json"));

	//トランジションエフェクト
	transitionEffect_ = std::make_unique<TransitionEffect>();
	transitionEffect_->Initialize();
	
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

	// タンク
	tank_ = std::make_unique<Tank>();
	tank_->Initialize(AnimationManager::Create("./resources/Tank", "Atlas.png", "tank.gltf"), "Tank");
	// レンジャー
	renju_ = std::make_unique<Renju>();
	renju_->Initialize(AnimationManager::Create("./resources/Renju", "Atlas.png", "renju.gltf"), "Renju");
	// ヒーラー
	healer_ = std::make_unique<Healer>();
	healer_->Initialize(AnimationManager::Create("./resources/Healer", "Atlas.png", "healer.gltf"), "Healer");

	//コマンド
	command_ = std::make_unique<Command>();
	command_->Initialize();

	
	//バトルのフラグ
	battle_ = false;

	//ラジアルブラー
	radialBlur_.isEnble = false;
	radialBlur_.blurWidth = GameSceneConstants::kRadialBlurInitWidth;
	radialBlur_.rotation = GameSceneConstants::kRadialBlurInitRotation;
	

	characters = {healer_.get(), renju_.get(),tank_.get()};

}

void GameScene::Update() {

	//遷移演出
	Effect();

	//敵の更新
	enemyManager_->Update();
	if (playerManager_->GetPlayer()->IsDead()) {
		enemyManager_->Stop();
	}

	//各キャラの情報の取得
	playerManager_->GetPlayer()->SetBarrierPos(tank_->GetBarrierWorldPos());

	//敵の情報取得
	playerManager_->GetPlayer()->SetEnemy(enemyManager_->GetEnemy());
	
	//playerの更新
	if (radialBlur_.blurWidth == GameSceneConstants::kRadialBlurInitWidth) {
		playerManager_->Update();
	}
	

	//コマンドの更新
	if (!playerManager_->GetPlayer()->IsOuto() && battle_) {
		command_->SetBehavior(playerManager_->GetPlayer()->IsRoot());
		command_->Update();
	}
	
	//コマンドの受取り
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
		//バリアを展開しているposの受け取り
		character->SetBarrierPos(tank_->GetBarrierWorldPos());
		//敵の子分の情報を格納する
		if (enemyManager_->GetEnemy()->IsSpecial()) {
			character->SetHenchman(enemyManager_->GetEnemy()->GetEnemys());
		}
	}

	//敵の子分の情報を格納する
	if (enemyManager_->GetEnemy()->IsSpecial()) {
		playerManager_->GetPlayer()->SetHenchmans(enemyManager_->GetEnemy()->GetEnemys());
	}

	
	//回復の適用
	ApplyHealing();
	

	//各キャラの更新
	if (!enemyManager_->IsClear()) {
		for (auto* character : characters) {
			character->Update();
		}
	}

	//レンジャーを守るためのpos
	tank_->SetPos(renju_->GetWorldPosition());

	//味方がバリアを張り始めているか
	healer_->SetBarrier(tank_->GetBarrier());
	renju_->SetBarrier(tank_->GetBarrier());
	healer_->SetBarrierThreshold(tank_->GetBarrierThreshold());
	renju_->SetBarrierThreshold(tank_->GetBarrierThreshold());

	//敵に必要な情報を渡す
	ConfigureEnemyState();
	
	skydome_->Update();
	loader_->Update();

	//当たり判定
	CheckAllCollision();

	//カメラやライティングの更新
	ViewConfig();
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
	tank_->Draw(viewProjection_);
	// ヒーラー
	healer_->Draw(viewProjection_);
	// レンジャー
	renju_->Draw(viewProjection_);
	
	//ダッシュではない時ポストエフェクトをかける
	if (!playerManager_->GetPlayer()->IsDash()) {
		//プレイヤー
		playerManager_->Draw(viewProjection_);
	}

	//バリアの描画
	enemyManager_->GetEnemy()->BarrierDraw(viewProjection_);
	tank_->BarrierDraw(viewProjection_);

#pragma endregion

#pragma region 前景スプライト描画
	
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

#pragma endregion
}

void GameScene::RenderDirect() {
	
	//ダッシュ中はポストエフェクトをかけない
	if (playerManager_->GetPlayer()->IsDash()) {
		playerManager_->Draw(viewProjection_);
	}

	if (!playerManager_->GetPlayer()->IsDash() && (!playerManager_->GetPlayer()->GameStart()|| (playerManager_->GetPlayer()->GameStart() && cameraDirection_))) {
		
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
	


	//デプスのないオブジェクト
	playerManager_->GetPlayer()->NoDepthDraw(viewProjection_);
	healer_->NoDepthDraw(viewProjection_);
	renju_->NoDepthDraw(viewProjection_);
	tank_->NoDepthDraw(viewProjection_);
	enemyManager_->NoDepthDraw(viewProjection_);
	
	//遷移演出の描画
	transitionEffect_->Draw();

}

void GameScene::BattleBegin(){
	PostEffect* const posteffect = PostEffect::GetInstance();
	//ラジアルブラーの演出
	if (playerManager_->GetPlayer()->GameStart() && !cameraDirection_ && radialBlur_.blurWidth <GameSceneConstants::kMaxRadialBlurWidth) {
		posteffect->isRadialBlur(true);

		radialBlur_.blurWidth = Math::LerpShortAngle(radialBlur_.blurWidth,1.0f,0.1f);
		cameraDirectionTime_ = GameSceneConstants::kCameraDirectionTimeMax;

		healer_->SetGameStart(true);
		renju_->SetGameStart(true);
		tank_->SetGameStart(true);
	}

	//カメラの演出に移行
	if (radialBlur_.blurWidth >= GameSceneConstants::kMaxRadialBlurWidth && !cameraDirection_) {
		--cameraDirectionTime_;
		if (cameraDirectionTime_ <= 0) {
			posteffect->isRadialBlur(false);
			radialBlur_.blurWidth = GameSceneConstants::kRadialBlurInitWidth;
			radialBlur_.rotation = 0.f;
			cameraDirection_ = true;
			//位置の初期化
			playerManager_->GetPlayer()->InitPos();
			healer_->InitPos(GameSceneConstants::kHealerInitPos);
			renju_->InitPos(GameSceneConstants::kRenjuInitPos);
			tank_->InitPos(GameSceneConstants::kTankInitPos);
			//カメラの初期化
			followCamera_->InitAngle();
			followCamera_->SetinterTargetPos(GameSceneConstants::kFollowCameraInit);
		}
	}

	posteffect->RadialBlurWidth(radialBlur_.blurWidth);
	posteffect->RadialBlurRotation(radialBlur_.rotation);

	//カメラが一定の位置に来たらバトル開始
	if (cameraDirection_ && !battle_) {
		followCamera_->CameraDirection();
		if (!followCamera_->GetMoveToEnemy()) {
			enemyManager_->GetEnemy()->SetAnimationNumber(kThreat, GameSceneConstants::kEnemyAnimationFlame,false);
		}

		//カメラがプレイヤーのとこまで来たらスタート
		if (followCamera_->GetViewProjection().translation_.z <= GameSceneConstants::kBattleStartZThreshold) {
			battle_ = true;
			command_->SetTaskType(TaskType::kInitial);
			command_->SetOperatin(false);
		}
	}
	
	//バトル開始のフラグ受け取り
	playerManager_->GetPlayer()->SetBattleStart(battle_);
	enemyManager_->GetEnemy()->SetBattleStart(battle_);
	healer_->SetBattleStart(battle_);
	renju_->SetBattleStart(battle_);
	tank_->SetBattleStart(battle_);
}

void GameScene::Effect(){
	//遷移演出の更新
	transitionEffect_->SetGameClear(enemyManager_->IsClear());
	transitionEffect_->SetGameOver(playerManager_->IsOver());
	transitionEffect_->Update();
	//エンカウント時の演出
	BattleBegin();
}

void GameScene::ApplyHealing(){
	//魔法陣に使う座標の受け取り
	healer_->SetPos(renju_->GetWorldPosition(), tank_->GetWorldPosition());
	renju_->SetTankPos(tank_->GetWorldPosition());
	//回復
	if (healer_->GetHeal()) {
		//全体回復
		if (healer_->GetAllHeal()) {
			playerManager_->GetPlayer()->SetHeal(healer_->GetHealAmount());
			for (auto* character : characters) {
				if (!character->IsDead()) {
					character->SetHeal(healer_->GetHealAmount());
				}
			}
		}
		//個人回復
		if (healer_->GetOneHeal()) {
			if (healer_->GetPlayerHp() <= HealerConstants::kSingleHealHpThreshold && playerManager_->GetPlayer()->GetHp() > 0) {
				playerManager_->GetPlayer()->SetHeal(healer_->GetHealAmount());
			}
			else {
				for (auto* character : characters) {
					if (character->GetHp() <= HealerConstants::kSingleHealHpThreshold && !character->IsDead()) {
						character->SetHeal(healer_->GetHealAmount());
						break;
					}
				}
			}
		}

		//個人回復(敵が子分を出す攻撃を出してきたとき用。回復させるタイミングが違うので二つ用意している。)
		if (enemyManager_->GetEnemy()->GetBehaviorAttack() == BehaviorAttack::kHenchman) {
			if (healer_->GetPlayerHp() <= HealerConstants::kHenchmanHealThreshold && playerManager_->GetPlayer()->GetHp() > 0) {
				playerManager_->GetPlayer()->SetHeal(healer_->GetHealAmount());
			}
			else {
				for (auto* character : characters) {
					if (character->GetHp() <= HealerConstants::kHenchmanHealThreshold && !character->IsDead()) {
						character->SetHeal(healer_->GetHealAmount());
						break;
					}
				}
			}
		}

	}

	//hp情報の取得
	if (healer_->GetState() != CharacterState::Unique) {
		healer_->SetHp({ playerManager_->GetPlayer()->GetHp(),renju_->GetHp(),tank_->GetHp() });
	}

}

void GameScene::ConfigureEnemyState(){
	//生存しているかどうか
	enemyManager_->GetEnemy()->SetIsDeadPlayer(playerManager_->GetPlayer()->GetIsDead());
	enemyManager_->GetEnemy()->SetIsDeadHealer(healer_->IsDead());
	enemyManager_->GetEnemy()->SetIsDeadRenju(renju_->IsDead());
	enemyManager_->GetEnemy()->SetIsDeadTank(tank_->IsDead());

	//プレイヤーと味方AI座標の取得
	enemyManager_->SetPlayerPos(playerManager_->GetPlayer()->GetWorldPosition());
	enemyManager_->SetHealerPos(healer_->GetWorldPosition());
	enemyManager_->SetRenjuPos(renju_->GetWorldPosition());
	enemyManager_->SetTankPos(tank_->GetWorldPosition());

	//ブレスパーティクルのフィールド用
	enemyManager_->GetEnemy()->SetTankRotation(tank_->GetWorldTransform().rotate);
	enemyManager_->GetEnemy()->SethmansRenjuPos(renju_->GetWorldPosition());

	//攻撃の受け取り
	if (healer_->IsAttack() && enemyManager_->GetEnemy()->GetBehaviorAttack() != BehaviorAttack::kHenchman) { enemyManager_->OnHealerCollision(); }
	if (tank_->IsAttack() && enemyManager_->GetEnemy()->GetBehaviorAttack() != BehaviorAttack::kHenchman) { enemyManager_->OnTankCollision(); }
	if (playerManager_->IsAttack()) { enemyManager_->OnCollision(); }
	if (renju_->GetHitBullet()) {
		enemyManager_->OnRenjuCollision(renju_->GetSkill());
		enemyManager_->GetEnemy()->SetRenjuSpecial(renju_->GetSpecial());
	}
}

void GameScene::ViewConfig(){
	// 追従カメラの更新
	if ((!cameraDirection_ || battle_) && radialBlur_.blurWidth == GameSceneConstants::kRadialBlurInitWidth && !playerManager_->IsOver()) {
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

void GameScene::CheckAllCollision() {
	//コリジョン関係
	collisionManager_->ClearColliderList();
	collisionManager_->SetColliderList(playerManager_->GetPlayer());
	collisionManager_->SetColliderList(enemyManager_->GetEnemy());
	if (enemyManager_->IsAttack()) {
		collisionManager_->SetColliderList(enemyManager_->GetEnemy()->GetRockCollider());

		for (int i = 0; i < 15; ++i) {
			collisionManager_->SetColliderList(enemyManager_->GetEnemy()->GetCollider(i));
		}
	}
	
	for (auto* character : characters) {
		collisionManager_->SetColliderList(character);
	}
	
	for (RenjuBullet* bullet : renju_->GetBullets()) {
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
	playerManager_->GetPlayer()->SetTankDead(tank_->IsDead());
	playerManager_->GetPlayer()->SetRenjuDead(renju_->IsDead());
	playerManager_->GetPlayer()->SetHealerDead(healer_->IsDead());

}