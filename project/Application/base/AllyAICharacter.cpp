#include "AllyAICharacter.h"


void AllyAICharacter::Initialize(Animations* animation, std::string className) {
	className_ = className;
	animation_ = animation;
	animationNumber_ = kStandby;
	flameTime_ = AllyAIConstants::kFlameTimeInit;

	shadowModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));
	shadowModel_->SetBlendMode(BlendMode::kSubtract);

	damageModel_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/character/20.png"));

	spriteHP_.reset(Sprite::Create("resources/Player/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/character/HPG.png"));
	spriteMP_.reset(Sprite::Create("resources/Player/MP.png"));
	spriteMPG_.reset(Sprite::Create("resources/character/HPG.png"));

	spriteH_.reset(Sprite::Create("resources/character/H.png"));
	spriteM_.reset(Sprite::Create("resources/character/M.png"));


	for (int i = 0; i < 3; ++i) {
		HPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		MPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		HPnum_[i]->SetSize(AllyAIConstants::kSpriteNumSize);
		MPnum_[i]->SetSize(AllyAIConstants::kSpriteNumSize);
	}

	HPnum_[2]->SetTexture("resources/character/1.png");
	MPnum_[2]->SetTexture("resources/character/1.png");

	spriteHPG_->SetSize(AllyAIConstants::kSpriteHPSize);
	spriteMPG_->SetSize(AllyAIConstants::kSpriteMPSize);
	spriteH_->SetSize(AllyAIConstants::kSpriteHSize);
	spriteM_->SetSize(AllyAIConstants::kSpriteMSize);

	InitializePerCharacter();

	spriteNumP_->SetSize(AllyAIConstants::kSpriteNumPSize);
	spriteName_->SetSize(AllyAIConstants::kSpriteNameSize);

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformNum_.Initialize();
	worldTransformShadow_.Initialize();
	worldTransformShadow_.rotate = AllyAIConstants::kShadowRotation;
	if (className != "Tank") {worldTransformShadow_.scale = AllyAIConstants::kShadowScaleDefault;}
	else { worldTransformShadow_.scale = AllyAIConstants::kShadowScaleTank; }
	worldTransformNum_.scale = AllyAIConstants::kWorldTransformNumScale;
	alpha_ = AllyAIConstants::kDamageInitAlpha;
	

	animation_->SetpreAnimationTimer(0.0f);
	animation_->SetAnimationTimer(0.0f, 0.0f);
	animation_->Update(animationNumber_);
}

void AllyAICharacter::Update() {

	//影の計算
	shadowColor_.w = 1 - (worldTransformBase_.translate.y / AllyAIConstants::kShadowAlphaAdjustment);
	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { worldTransformBase_.translate.x,AllyAIConstants::kShadowTranslateOffset,worldTransformBase_.translate.z };
	worldTransformShadow_.UpdateMatrix();



	//体力がなくなあったら強制的に死亡に状態遷移
	if (hp_ <= 0) {
		hp_ = 0;
		if (NextState("Move", Output6) == CharacterState::Dead ||
			NextState("Attack", Output3) == CharacterState::Dead ||
			NextState("Jump", Output2) == CharacterState::Dead ||
			NextState(skillName_, Output2) == CharacterState::Dead||
			NextState("Barrier", Output2) == CharacterState::Dead || 
			NextState("Protect", Output2) == CharacterState::Dead ) {
			state_ = CharacterState::Dead;
		}

	}

	//各キャラの処理にしないといけない
	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kNomal && GetAimCharacter()) {
		if (enemy_->IsAttack()) {
			isHit_ = true;
			if (isHit_ != preHit_) {
				hp_ -= AllyAIConstants::kEnemyDamageNormal;
				DameageInit();
				damageModel_->SetTexture("character/10.png");
			}
		}
	}

	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman) {
		if (enemy_->IsAttack()) {
			isHit_ = true;
			if (isHit_ != preHit_) {
				hp_ -= AllyAIConstants::kEnemyDamageHenchman;
				DameageInit();
				damageModel_->SetTexture("character/50.png");
				//失敗したら強制的に状態遷移
				if (!isDead_) {
					if(className_ != "Renju")
					state_ = NextState("Protect", Output1);
				}
				else {
					state_ = NextState(skillName_, Output1);
				}
				
			}
		}
	}

	//ダメージの表示
	damageModel_->SetColor({ 1.f,1.f,1.f,alpha_ });
	if (alpha_ > 0.0f) {
		alpha_ -= AllyAIConstants::kDamageAlphaDecrement;
		worldTransformNum_.translate = Math::Lerp(worldTransformNum_.translate, { numMove_ }, 0.05f);
	}
	else {
		worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + AllyAIConstants::kDamageDisplayHeight,worldTransformBase_.translate.z };
	}

	//barrierの中にいるか
	BarrierRange();

	// 回転
	worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
	animation_->SetFlameTimer(flameTime_);
	animation_->Update(animationNumber_);
	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.TransferMatrix();
	worldTransformNum_.TransferMatrix();

	spriteHP_->SetSize({ hp_,AllyAIConstants::kSpriteHPSize.y});
	spriteMP_->SetSize({ mp_,AllyAIConstants::kSpriteMPSize.y});
	//HP,MP表示の計算
	if (hp_ < AllyAIConstants::kMaxHPMPDisplay) {
		int HPnum = int(hp_);
		HPnum %= 10;//桁をずらす
		if (hp_ >= 0) {
			HPnum_[0]->SetTexture("resources/character/" + std::to_string(HPnum) + ".png");
			HPnum_[1]->SetTexture("resources/character/" + std::to_string(int(hp_ / 10)) + ".png");
		}
		else {
			HPnum_[0]->SetTexture("resources/character/0.png");
			HPnum_[1]->SetTexture("resources/character/0.png");
		}

	}
	else {
		HPnum_[0]->SetTexture("resources/character/0.png");
		HPnum_[1]->SetTexture("resources/character/0.png");
	}


	if (mp_ < AllyAIConstants::kMaxHPMPDisplay) {
		int MPnum = int(mp_);
		MPnum %= 10;//桁をずらす
		MPnum_[0]->SetTexture("resources/character/" + std::to_string(MPnum) + ".png");
		MPnum_[1]->SetTexture("resources/character/" + std::to_string(int(mp_ / 10)) + ".png");
	}
	else {
		MPnum_[0]->SetTexture("resources/character/0.png");
		MPnum_[1]->SetTexture("resources/character/0.png");
	}

	spriteHP_->SetColor(hpColor_);
	for (int i = 0; i < 3; ++i) {
		HPnum_[i]->SetColor(hpNumColor_);
		MPnum_[i]->SetColor(hpNumColor_);
	}


	//色の設定
	if (hp_ < AllyAIConstants::kLowHPThreshold) {
		hpColor_ = AllyAIConstants::kLowHPColor;
		hpNumColor_ = AllyAIConstants::kLowHPColor;

	}
	else if (hp_ <= AllyAIConstants::kMediumHPThreshold) {
		hpNumColor_ = AllyAIConstants::kMediumHPColor;
		hpColor_ = AllyAIConstants::kDefaultHPColor;
	}
	else {
		hpColor_ = AllyAIConstants::kDefaultHPColor;
		hpNumColor_ = AllyAIConstants::kDefaultHPColor;
	}



}

void AllyAICharacter::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformBody_, camera, true);
	shadowModel_->Draw(worldTransformShadow_, camera, false);
}
void AllyAICharacter::NoDepthDraw(const ViewProjection& camera) {
	if (!isDead_) {
		damageModel_->Draw(worldTransformNum_, camera, false);
	}
	
}

void AllyAICharacter::DrawUI() {
	spriteHPG_->Draw();
	spriteHP_->Draw();
	spriteMPG_->Draw();
	spriteMP_->Draw();
	spriteNumP_->Draw();
	spriteH_->Draw();
	spriteM_->Draw();
	spriteName_->Draw();

	for (int i = 0; i < 2; ++i) {
		HPnum_[i]->Draw();
		MPnum_[i]->Draw();
	}

	if (hp_ == 100) {
		HPnum_[2]->Draw();
	}
	if (mp_ == 100) {
		MPnum_[2]->Draw();
	}
}

void AllyAICharacter::MoveInitialize()
{
	//目標座標を-8～8の範囲でランダムにずらす
	randPos_ = Vector3{ RandomGenerator::GetRandomFloat(-8.f, 8.f),0,RandomGenerator::GetRandomFloat(-8.f, 8.f) };
	worldTransformBase_.translate.y = 0.0f;
	velocity_ = { 0.0f,0.0f,0.0f };
	searchTarget_ = false;
	isAttack_ = false;
	drawWepon_ = false;
	animation_->SetpreAnimationTimer(0);
	animation_->SetLoop(true);
	flameTime_ = AllyAIConstants::kFlameTimeInit;
	animationNumber_ = kStandby;

}
void AllyAICharacter::MoveUpdate() {


	--coolTime_;
	// 敵の座標までの距離
	enemylength_ = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));


	// プレイヤーに集合
	if ((operation_ || !searchTarget_) && (!gameStart_ || battleStart_)) {
		followPlayer_ = true;
		searchTarget_ = false;
		followPlayer();
	}

	// 敵を探す
	if (!operation_) {
		searchTarget_ = true;
		followPlayer_ = false;
		searchTarget();
	}

	if (isArea_ && searchTarget_ && enemy_->IsAreaDraw()) {
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(AllyAIConstants::kCharacterSpeed, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		worldTransformBase_.translate += velocity_;
		animationNumber_ = kRun;
	}
	IsVisibleToEnemy();




	//敵の攻撃が終わったらまたジャンプできるように設定
	if (!enemy_->IsAttack()) {
		jumpCount_ = 1;
	}

	//地面をたたきつける攻撃が来たらジャンプする
	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kGround && enemy_->GetBehavior() == Behavior::kAttack && enemy_->IsAttack()) {
		//ジャンプは敵の攻撃一回に対して一回まで
		if (jumpCount_ == 1 && enemylength_ <= AllyAIConstants::kGroundAttackDistance) {
			//敵との距離とimpactのサイズに応じてジャンプするタイミングをずらす

			if (enemylength_ < AllyAIConstants::kJumpDistanceNear && enemy_->GetImpactSize() < AllyAIConstants::kImpactSizeSmall) {
				state_ = NextState("Move", Output2);
			}

			if (Math::isWithinRange(enemylength_, AllyAIConstants::kJumpDistanceMidLow, 5) && Math::isWithinRange(enemy_->GetImpactSize(), AllyAIConstants::kImpactSizeMediumLow, 10)) {
				state_ = NextState("Move", Output2);
			}

			if (Math::isWithinRange(enemylength_, AllyAIConstants::kJumpDistanceMidHigh, 5) && Math::isWithinRange(enemy_->GetImpactSize(), AllyAIConstants::kImpactSizeMediumHigh, 10)) {
				state_ = NextState("Move", Output2);
			}

			if (Math::isWithinRange(enemylength_, AllyAIConstants::kJumpDistanceFar, 5) && Math::isWithinRange(enemy_->GetImpactSize(), AllyAIConstants::kImpactSizeLarge, 10)) {
				state_ = NextState("Move", Output2);
			}
		}

	}

	//ブレス攻撃
	if (barrier_ && barrierThreshold_ <= 0.0f) {
		state_ = NextState("Move", Output4);
	}

	//弓キャラを守る
	if (className_ != "Renju") {
		if (enemy_->GetBehavior() == Behavior::kAttack && enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman && state_ != CharacterState::Protect) {
			if (henchmans_.size() >= AllyAIConstants::kHenchmanCountThreshold) {
				henchmanSearch_ = true;
				state_ = NextState("Move", Output5);
			}
		}
	}

}

void AllyAICharacter::JumpInitialize()
{
	--jumpCount_;
	worldTransformBase_.translate.y = 0.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	velocity_ = { 0.f,kJumpFirstSpeed,0.f };
	animationNumber_ = kJump;
	flameTime_ = AllyAIConstants::kFlameTimeInit;
	animation_->SetAnimationTimer(0.5f, flameTime_);
	animation_->SetLoop(false);
}
void AllyAICharacter::JumpUpdate()
{
	// 移動
	worldTransformBase_.translate += velocity_;
	// 重力加速度
	const float kGravity = 0.05f;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;

	if (worldTransformBase_.translate.y <= 0.0f) {
		// ジャンプ終了
		state_ = NextState("Jump", Output1);
		velocity_.y = 0.0f;
	}
}

void AllyAICharacter::AttackInitialize()
{
	searchTarget_ = false;
	animationNumber_ = kAnimeAttack;
	drawWepon_ = true;
}
void AllyAICharacter::AttackUpdate()
{
	if (!isAttack_) {
		//地面をたたきつける攻撃が来たらジャンプする
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kGround && enemy_->GetBehavior() == Behavior::kAttack && enemy_->IsAttack()) {
			//ジャンプは敵の攻撃一回に対して一回まで
			if (jumpCount_ == 1 && enemylength_ <= AllyAIConstants::kGroundAttackDistance) {
				//敵との距離とimpactのサイズに応じてジャンプするタイミングをずらす

				if (enemylength_ < AllyAIConstants::kJumpDistanceNear && enemy_->GetImpactSize() < AllyAIConstants::kImpactSizeSmall) {
					state_ = NextState("Move", Output2);
				}

				if (Math::isWithinRange(enemylength_, AllyAIConstants::kJumpDistanceMidLow, 5) && Math::isWithinRange(enemy_->GetImpactSize(), AllyAIConstants::kImpactSizeMediumLow, 10)) {
					state_ = NextState("Move", Output2);
				}

				if (Math::isWithinRange(enemylength_, AllyAIConstants::kJumpDistanceMidHigh, 5) && Math::isWithinRange(enemy_->GetImpactSize(), AllyAIConstants::kImpactSizeMediumHigh, 10)) {
					state_ = NextState("Move", Output2);
				}

				if (Math::isWithinRange(enemylength_, AllyAIConstants::kJumpDistanceFar, 5) && Math::isWithinRange(enemy_->GetImpactSize(), AllyAIConstants::kImpactSizeLarge, 10)) {
					state_ = NextState("Move", Output2);
				}
			}

		}
	}
	

	// プレイヤーに集合
	if (operation_) {
		state_ = NextState("Attack", Output1);
		isAttack_ = false;
		followPlayer_ = true;
		searchTarget_ = false;
		animation_->SetLoop(true);
	}
}

void AllyAICharacter::BreathInitialize() {
	velocity_ = { 0.0f,0.0f,0.0f };
	isAttack_ = false;
}
void AllyAICharacter::BreathUpdate() {
	// 追従対象からロックオン対象へのベクトル
	Vector3 sub = tankPos_ - GetWorldPosition();

	//Y軸の回転
	DestinationAngle(sub);

	
	// 敵の位置から自分の位置への方向ベクトルを計算
	Vector3 direction = tankPos_ - enemy_->GetWorldTransform().translate;

	// 方向ベクトルを反転させることで敵から遠ざかる方向に移動
	Math::Normalize(direction);   // 正規化して単位ベクトルにする
	direction *= -1.0f; // 反転して反対方向に進む

	

	
	Vector3 directionTank = (tankPos_ -(direction * AllyAIConstants::kEscapeFactor)) - GetWorldPosition();

	velocity_ = Math::Normalize(directionTank) * AllyAIConstants::kCharacterSpeed;

	//目的の場所までこれたか
	if (Math::isWithinRange(worldTransformBase_.translate.x, (tankPos_.x - (direction.x * AllyAIConstants::kEscapeFactor)), AllyAIConstants::kCloseDistance) &&
		Math::isWithinRange(worldTransformBase_.translate.z, (tankPos_.z - (direction.z * AllyAIConstants::kEscapeFactor)), AllyAIConstants::kCloseDistance)) {
		animationNumber_ = kStandby;
	}
	else {
		animationNumber_ = kRun;
		worldTransformBase_.translate += velocity_;
		worldTransformBase_.translate.y = 0;
	}


	if (!barrier_ || barrierThreshold_ > AllyAIConstants::kBarrierThreshold) {
		state_ = NextState("Breath", Output1);
	}
}

void AllyAICharacter::ProtectInitialize()
{
	velocity_ = { 0.0f,0.0f,0.0f };
	isAttack_ = false;
	animationNumber_ = kRun;
}
void AllyAICharacter::ProtectUpdate()
{
	if (!currentTarget_ || currentTarget_->IsDead()) {
		for (EnemyHenchman* enemy : henchmans_) {
			if (!enemy->IsDead()) {
				currentTarget_ = enemy;
				henchmanSearch_ = false;
			}
		}
	}
	
	//キャラ後tのに動き方を変える
	if (className_ == "Tank") {
		//レンジャーから最も近い敵を倒す
		for (EnemyHenchman* enemy : henchmans_) {
			if (!enemy->IsDead() && !enemy->IsTarget() && !enemy->IsHitEnemy() && enemy) {
				if (enemy->GetPos().y >= 0) {
					float distanceToNowDist = Math::GetDistanceSquared(renjuPos_, henchmanDist_);
					float distanceToHench = Math::GetDistanceSquared(renjuPos_, enemy->GetPos());
					//今追いかけている子分より更にレンジャーに近い子分がいたらそっちを追いかける
					if (distanceToNowDist > distanceToHench) {

						currentTarget_ = enemy;

					}
				}
			}

		}

	}
	else {
		//自分から最も近い敵を倒す
		for (EnemyHenchman* enemy : henchmans_) {

			if (!enemy->IsDead() && !enemy->IsTarget() && !enemy->IsHitEnemy() && enemy) {
				if (enemy->GetPos().y >= 0) {
					float distanceToNowDist = Math::GetDistanceSquared(worldTransformBase_.translate, henchmanDist_);
					float distanceToHench = Math::GetDistanceSquared(worldTransformBase_.translate, enemy->GetPos());
					//今追いかけている子分より更に自分に近い子分がいたらそっちを追いかける
					if (distanceToNowDist > distanceToHench) {

						currentTarget_ = enemy;

					}
				}
			}
		}
	}



	if (currentTarget_ && !currentTarget_->IsDead()) {
		henchmanDist_ = currentTarget_->GetWorldPosition();
	}

	//一番近い子分の方を向く// 追従対象からロックオン対象へのベクトル
	Vector3 sub = henchmanDist_ - GetWorldPosition();

	//Y軸の回転
	DestinationAngle(sub);

	const float kSpeed = 0.2f;
	Vector3 direction = sub;

	velocity_ = Math::Normalize(direction) * kSpeed;

	worldTransformBase_.translate += velocity_;
	worldTransformBase_.translate.y = 0.0f;

	float sub_ = Math::Length(sub);

	//攻撃する
	if (AllyAIConstants::kHenchmanMinDistance_ >= sub_){
		//攻撃アクションに移行
		drawWepon_ = true;
		animationNumber_ = kStandby;
		state_ = CharacterState::Attacking;
	}

	//Moveに戻る
	if (enemy_->GetBehavior() != Behavior::kAttack) {
		state_ = NextState("Protect", Output1);
		currentTarget_ = nullptr;
	}
}

void AllyAICharacter::DeadInitialize()
{
	isAttack_ = false;
	isDead_ = true;
	animationNumber_ = kDeath;
	animation_->SetLoop(false);
	animation_->SetpreAnimationTimer(0.0f);
	animation_->SetAnimationTimer(0.0f, 0.0f);
}

void AllyAICharacter::Relationship()
{
	worldTransformBody_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformBody_.scale, worldTransformBody_.rotate, worldTransformBody_.translate),
		worldTransformBase_.matWorld_);

	backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrixNum = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
	billboardMatrixNum.m[3][0] = worldTransformNum_.translate.x;
	billboardMatrixNum.m[3][1] = worldTransformNum_.translate.y;
	billboardMatrixNum.m[3][2] = worldTransformNum_.translate.z;
	worldTransformNum_.matWorld_ = Math::MakeScaleMatrix(worldTransformNum_.scale) * billboardMatrixNum;

}

void AllyAICharacter::followPlayer()
{
	if (followPlayer_ && state_ != CharacterState::Unique && state_ != CharacterState::Breath) {

		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = playerPos_ - GetWorldPosition();

		//Y軸の回転
		DestinationAngle(sub);

		// プレイヤーの座標までの距離
		float length = Math::Length(Math::Subract(playerPos_, worldTransformBase_.translate));

		// 距離条件チェック
		if (minDistance_ <= length) {
			drawWepon_ = false;
			
			Vector3 direction = sub;

			velocity_ = Math::Normalize(direction) * AllyAIConstants::kCharacterSpeed;

			worldTransformBase_.translate += velocity_;
			animationNumber_ = kRun;
			worldTransformBase_.translate.y = 0.0f;
		}
		else {
			followPlayer_ = false;
			animationNumber_ = kStandby;
		}
	}
}

void AllyAICharacter::searchTarget()
{
	if (!followPlayer_ && searchTarget_ && state_ != CharacterState::Breath) {
		drawWepon_ = false;
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

		//Y軸の回転
		DestinationAngle(sub);

		// 距離条件チェック
		if (minDistance_  <= enemylength_) {
			if (state_ != CharacterState::Jumping) {
				if (enemy_->GetBehaviorAttack() != BehaviorAttack::kDash || !enemy_->IsBehaberAttack()) {
					
					Vector3 direction = sub;

					velocity_ = Math::Normalize(direction) * AllyAIConstants::kCharacterSpeed;

					worldTransformBase_.translate += velocity_;
					animationNumber_ = kRun;
					worldTransformBase_.translate.y = 0.0f;
				}
			
			}

		}
		else {
			//攻撃アクションに移行
			drawWepon_ = true;
			animationNumber_ = kStandby;
			if (coolTime_ <= 0 && !isArea_ && enemy_->GetBehavior() != Behavior::kDead) {
				if (enemy_->GetBehaviorAttack() != BehaviorAttack::kDash) {
					state_ = NextState("Move", Output1);
				}
				else {
					if (!enemy_->IsBehaberAttack()&&!enemy_->IsAttack()|| jumpCount_ == 0) {
						state_ = NextState("Move", Output1);
					}

				}

			}

		}
	}
}

void AllyAICharacter::IsVisibleToEnemy()
{
	isArea_ = false;
	//float rectWidth = 6.0f; // 横幅の設定 (敵の中心から±3)
	Vector3 toEnemy = enemy_->GetWorldPosition() - worldTransformBase_.translate;
	// 敵の視線方向を取得 (Z軸方向が前方)
	Vector3 enemyForward = {
		enemy_->GetWorldTransform().matWorld_.m[2][0],
		enemy_->GetWorldTransform().matWorld_.m[2][1],
		enemy_->GetWorldTransform().matWorld_.m[2][2]
	};
	enemyForward *= -1;

	Vector3 enemyRight = Math::Cross({ 0.0f, 1.0f, 0.0f }, enemyForward);
	enemyRight = Math::Normalize(enemyRight);

	//敵との距離
	float distance = Math::Length(toEnemy);


	//距離条件のチェック
	if (AllyAIConstants::kEnemyMinDistance_ <= distance && distance <= AllyAIConstants::kEnemyMaxDistance_) {
		toEnemy = Math::Normalize(toEnemy); // toEnemyベクトルを正規化
		enemyForward = Math::Normalize(enemyForward); // enemyForwardベクトルを正規化

		float dot = Math::Dot(toEnemy, enemyForward);
		float angle = std::acos(dot);

		//角度条件チェック
		if (std::abs(angle) <= AllyAIConstants::kAngleRange_) {
			if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash && enemy_->IsAreaDraw()) {
				RunAway();
				isArea_ = true;
			}

		}
	}
}

void AllyAICharacter::BarrierRange() {
	//バリアの範囲内にいるか
	if (enemy_->IsAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kBreath) {
		//プレイヤーの円
		Circle p;
		p.center = Vector2{ worldTransformBase_.translate.x,worldTransformBase_.translate.z };
		p.radius = 1.0f;

		//barrierの円
		Circle b;
		b.center = Vector2{ barrierPos_.x, barrierPos_.z };
		b.radius = 7.0f;

		if (!Math::CircleColiision(p, b) && alpha_ <= 0.0f) {
			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= AllyAIConstants::kEnemyDamageBreath;
				DameageInit();
				damageModel_->SetTexture("character/20.png");
			}
		}
	}

}

void AllyAICharacter::DestinationAngle(Vector3 sub){
	// y軸周りの回転
	if (sub.z != 0.0) {
		destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

		if (sub.z < 0.0) {
			destinationAngleY_ = (sub.x >= 0.0)
				? std::numbers::pi_v<float> -destinationAngleY_
				: -std::numbers::pi_v<float> -destinationAngleY_;
		}
	}
	else {
		destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f
			: -std::numbers::pi_v<float> / 2.0f;
	}
}

void AllyAICharacter::RunAway()
{
	animationNumber_ = kRun;
	//敵の攻撃範囲からどちらに逃げる方が早く範囲外に出れるか
	if (enemy_->GetWorldPosition().z > worldTransformBase_.translate.z) {
		if (enemy_->GetWorldPosition().x > worldTransformBase_.translate.x) {
			velocity_ = { -AllyAIConstants::kVelocityX,0.0f,AllyAIConstants::kVelocityZ };
		}
		else {
			velocity_ = { AllyAIConstants::kVelocityX,0.0f,AllyAIConstants::kVelocityZ };
		}
	}
	else {
		if (enemy_->GetWorldPosition().x < worldTransformBase_.translate.x) {
			velocity_ = { -AllyAIConstants::kVelocityX,0.0f,AllyAIConstants::kVelocityZ };
		}
		else {
			velocity_ = { AllyAIConstants::kVelocityX,0.0f,AllyAIConstants::kVelocityZ };
		}
	}

}

CharacterState AllyAICharacter::NextState(std::string name, int outputNum)
{
	auto linkedNode = editor_.GetLinkNode(name, outputNum);

	// 状態に応じてノードの色を変更
	editor_.SetCurrentStateNode(linkedNode.id);


	if (linkedNode.name == "Move") {
		return CharacterState::Moveing;
	}
	else if (linkedNode.name == "Attack") {
		return CharacterState::Attacking;
	}
	else if (linkedNode.name == "Jump") {
		return CharacterState::Jumping;
	}
	else if (linkedNode.name == skillName_) {
		return CharacterState::Unique;
	}
	else if (linkedNode.name == "Dead") {
		return CharacterState::Dead;
	}
	else if (linkedNode.name == "Breath") {
		return CharacterState::Breath;
	}
	else if (linkedNode.name == "Protect") {
		return CharacterState::Protect;
	}
	else {
		return CharacterState::Moveing; // デフォルトの状態
	}
}

void AllyAICharacter::InitializePerCharacter()
{

	if (className_ == "Healer") {
		skillName_ = "Heal";
		spriteNumP_.reset(Sprite::Create("resources/character/4P.png"));
		spriteName_.reset(Sprite::Create("resources/Healer/healer.png"));
		spriteHP_->SetPosition(Vector2{ AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kHealerSpriteHPBarYPos});
		spriteHPG_->SetPosition(Vector2{ AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kHealerSpriteHPBarYPos});
		spriteMP_->SetPosition(Vector2{ AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kHealerSpriteMPBarYPos});
		spriteMPG_->SetPosition(Vector2{ AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kHealerSpriteMPBarYPos});
		spriteNumP_->SetPosition(Vector2{AllyAIConstants::kSpriteIdentifyXPos,AllyAIConstants::kHealerSpriteNumPYPos });
		spriteH_->SetPosition(Vector2{AllyAIConstants::kSpriteIconXPos,AllyAIConstants::kHealerSpriteHIconYPos});
		spriteM_->SetPosition(Vector2{AllyAIConstants::kSpriteIconXPos,AllyAIConstants::kHealerSpriteMIconYPos});
		spriteName_->SetPosition(Vector2{AllyAIConstants::kSpriteIdentifyXPos,AllyAIConstants::kHealerSpriteNameYPos });
		for (int i = 0; i < 3; ++i) {
			HPnum_[i]->SetPosition(Vector2{AllyAIConstants::kNumXPos - (AllyAIConstants::kNumXOffset * i) ,AllyAIConstants::kHealerHPNumYPos});
			MPnum_[i]->SetPosition(Vector2{AllyAIConstants::kNumXPos - (AllyAIConstants::kNumXOffset * i) ,AllyAIConstants::kHealerMPNumYPos});
		}
	}
	else if (className_ == "Renju") {
		skillName_ = "Skill";
		spriteNumP_.reset(Sprite::Create("resources/character/3P.png"));
		spriteName_.reset(Sprite::Create("resources/Renju/renju.png"));
		spriteHP_->SetPosition(Vector2{AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kRenjuSpriteHPBarYPos });
		spriteHPG_->SetPosition(Vector2{AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kRenjuSpriteHPBarYPos });
		spriteMP_->SetPosition(Vector2{AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kRenjuSpriteMPBarYPos });
		spriteMPG_->SetPosition(Vector2{AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kRenjuSpriteMPBarYPos });
		spriteNumP_->SetPosition(Vector2{AllyAIConstants::kSpriteIdentifyXPos,AllyAIConstants::kRenjuSpriteNumPYPos });
		spriteH_->SetPosition(Vector2{AllyAIConstants::kSpriteIconXPos,AllyAIConstants::kRenjuSpriteHIconYPos });
		spriteM_->SetPosition(Vector2{AllyAIConstants::kSpriteIconXPos,AllyAIConstants::kRenjuSpriteMIconYPos });
		spriteName_->SetPosition(Vector2{AllyAIConstants::kSpriteIdentifyXPos,AllyAIConstants::kRenjuSpriteNameYPos });
		for (int i = 0; i < 3; ++i) {
			HPnum_[i]->SetPosition(Vector2{AllyAIConstants::kNumXPos - (AllyAIConstants::kNumXOffset * i) ,AllyAIConstants::kRenjuHPNumYPos });
			MPnum_[i]->SetPosition(Vector2{AllyAIConstants::kNumXPos - (AllyAIConstants::kNumXOffset * i) ,AllyAIConstants::kRenjuMPNumYPos });
		}
	}
	else if (className_ == "Tank") {
		skillName_ = "Skill";
		spriteName_.reset(Sprite::Create("resources/Tank/tank.png"));
		spriteNumP_.reset(Sprite::Create("resources/character/2P.png"));
		spriteHP_->SetPosition(Vector2{AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kTankSpriteHPBarYPos });
		spriteHPG_->SetPosition(Vector2{AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kTankSpriteHPBarYPos });
		spriteMP_->SetPosition(Vector2{AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kTankSpriteMPBarYPos });
		spriteMPG_->SetPosition(Vector2{AllyAIConstants::kSpriteBarXPos,AllyAIConstants::kTankSpriteMPBarYPos });
		spriteNumP_->SetPosition(Vector2{AllyAIConstants::kSpriteIdentifyXPos,AllyAIConstants::kTankSpriteNumPYPos });
		spriteH_->SetPosition(Vector2{AllyAIConstants::kSpriteIconXPos,AllyAIConstants::kTankSpriteHIconYPos });
		spriteM_->SetPosition(Vector2{AllyAIConstants::kSpriteIconXPos,AllyAIConstants::kTankSpriteMIconYPos });
		spriteName_->SetPosition(Vector2{AllyAIConstants::kSpriteIdentifyXPos,AllyAIConstants::kTankSpriteNameYPos });
		for (int i = 0; i < 3; ++i) {
			HPnum_[i]->SetPosition(Vector2{AllyAIConstants::kNumXPos - (AllyAIConstants::kNumXOffset * i) ,AllyAIConstants::kTankHPNumYPos });
			MPnum_[i]->SetPosition(Vector2{AllyAIConstants::kNumXPos - (AllyAIConstants::kNumXOffset * i) ,AllyAIConstants::kTankMPNumYPos });
		}
	}

}

bool AllyAICharacter::GetAimCharacter()
{
	if (className_ == "Healer") {
		return enemy_->GetAimHealer();
	}
	else if (className_ == "Renju") {
		return enemy_->GetAimRenju();
	}
	else if (className_ == "Tank") {
		return enemy_->GetAimTank();
	}
	else {
		return false;
	}

}



void AllyAICharacter::OnCollision(Collider* collider)
{
	//敵との当たり判定
	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {

		henchmanSearch_ = true;

		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
			if (enemy_->IsAttack()) {

				isHit_ = true;

				if (isHit_ != preHit_) {
					if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
						hp_ -= AllyAIConstants::kEnemyDamageNormal;
						DameageInit();
						damageModel_->SetTexture("character/10.png");
					}
				}

			}
		}

		OBB obb = {
			.center{collider->GetOBB().center.x + collider->GetWorldPosition().x,collider->GetOBB().center.y + collider->GetWorldPosition().y,collider->GetOBB().center.z + collider->GetWorldPosition().z},

			.orientations{
			 {Vector3{collider->GetWorldTransform().matWorld_.m[0][0],collider->GetWorldTransform().matWorld_.m[0][1],collider->GetWorldTransform().matWorld_.m[0][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[1][0],collider->GetWorldTransform().matWorld_.m[1][1],collider->GetWorldTransform().matWorld_.m[1][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[2][0],collider->GetWorldTransform().matWorld_.m[2][1],collider->GetWorldTransform().matWorld_.m[2][2]}},
			},
			.size{collider->GetOBB().size}
		};

		worldTransformBase_.translate += Math::PushOutAABBOBB(worldTransformBase_.translate, GetAABB(), collider->GetWorldTransform().translate, obb);
	}

	//敵の攻撃との当たり判定
	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemyAttack) {
		if (enemy_->IsAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kGround) {

			isHit_ = true;

			if (isHit_ != preHit_) {
				jumpCount_ -= 1;
				hp_ -= AllyAIConstants::kEnemyDamageBreath;
				DameageInit();
				damageModel_->SetTexture("character/20.png");
			}

		}

		if (enemy_->IsAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kThrowing) {
			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= AllyAIConstants::kEnemyDamageNormal;
				DameageInit();
				damageModel_->SetTexture("character/10.png");
			}
		}
	}

	//壁との当たり判定
	if (collider->GetCollisionAttribute() == kCollisionAttributeLoderWall) {
		OBB obb = {
			.center{collider->GetOBB().center.x + collider->GetWorldPosition().x,collider->GetOBB().center.y + collider->GetWorldPosition().y,collider->GetOBB().center.z + collider->GetWorldPosition().z},

			.orientations{
			 {Vector3{collider->GetWorldTransform().matWorld_.m[0][0],collider->GetWorldTransform().matWorld_.m[0][1],collider->GetWorldTransform().matWorld_.m[0][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[1][0],collider->GetWorldTransform().matWorld_.m[1][1],collider->GetWorldTransform().matWorld_.m[1][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[2][0],collider->GetWorldTransform().matWorld_.m[2][1],collider->GetWorldTransform().matWorld_.m[2][2]}},
			},
			.size{collider->GetOBB().size}
		};
		worldTransformBase_.translate += Math::PushOutAABBOBB(worldTransformBase_.translate, GetAABB(), collider->GetWorldTransform().translate, obb);
	}

	//子分に当たった時の処理
	if (collider->GetCollisionAttribute() == kCollisionAttributeHenchman) {
		henchmanSearch_ = true;
	}

	worldTransformBase_.UpdateMatrix();
}

const Vector3 AllyAICharacter::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}
