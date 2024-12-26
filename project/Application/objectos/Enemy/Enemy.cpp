#include "Enemy.h"
#include <CollisionManager/CollisionConfig.h>
#include <ParticleManager.h>


void Enemy::Initialize() {


	animation_ = AnimationManager::Create("resources/Enemy", "Atlas_Monsters.png", "Alien.gltf");
	henchman_ = AnimationManager::Create("resources/Enemy", "Atlas_Monsters.png", "Alien2.gltf");
	impactModel_.reset(Model::CreateModelFromObj("resources/Enemy/impact.obj", "resources/white.png"));
	areaModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Enemy/red_.png"));
	circleAreaModel_.reset(Model::CreateModelFromObj("resources/Enemy/area.obj", "resources/Enemy/red_.png"));
	animation_->SetThreshold(threshold_);
	areaModel_->SetBlendMode(BlendMode::kNormal);
	circleAreaModel_->SetBlendMode(BlendMode::kNormal);
	areaModel_->SetColor({ 1.f,1.f,1.f,areaAlpha_ });
	circleAreaModel_->SetColor({ 1.f,1.f,1.f,areaAlpha_ });

	worldTransformBarrier_.Initialize();
	
	worldTransformBarrier_.scale = EnemyConstants::kBarrierScale;
	barrierModel_.reset(Model::CreateModelFromObj("resources/Tank/barrier.obj", "resources/white.png"));
	barrierModel_->SetMaskTexture("barrier.png");
	barrierModel_->SetBlendMode(BlendMode::kAdd);
	maskUV_.scale = EnemyConstants::kBarrierMaskUVScale;
	barrierModel_->SetMaskUV(maskUV_);
	barrierModel_->SetColor(EnemyConstants::kBarrierColor);

	areaModel_->DirectionalLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 2.0f, 0.0f }, 1.0f);
	animationNumber_ = kStandby;
	

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate = EnemyConstants::kBaseTranslate;
	worldTransformBase_.rotate = EnemyConstants::kBaseRotate;
	worldTransformBody_.Initialize();
	worldTransformBody_.scale = EnemyConstants::kBodyScale;
	worldTransformRock_.Initialize();
	worldTransformRock_.translate = EnemyConstants::kRockTranslate;
	worldTransformArea_.Initialize();
	worldTransformArea_.translate = EnemyConstants::kAreaTranslate;
	worldTransformArea_.scale = EnemyConstants::kAreaScale;
	worldTransformArea_.rotate = EnemyConstants::kAreaRotate;
	worldTransformCircleArea_.Initialize();
	worldTransformCircleArea_.scale = EnemyConstants::kCircleAreaScale;
	worldTransformImpact_.Initialize();
	//衝撃波の当たり判定の設定
	for (int i = 0; i < EnemyConstants::kImpactColliderCount; ++i) {
		worldTransformColliderImpact_[i].Initialize();
		AABB aabb = { {-7.0f,-0.2f,-1 },{7.0f,0.2f,1} };
		OBB obb = Math::ConvertAABBToOBB(aabb);
		colliderManager_[i] = std::make_unique<ColliderManager>();
		colliderManager_[i]->SetOBB(obb);
		colliderManager_[i]->SetCollisionMask(kCollisionMaskEnemyAttack);
		colliderManager_[i]->SetCollisionAttribute(kCollisionAttributeEnemyAttack);
		colliderManager_[i]->SetCollisionPrimitive(kCollisionPrimitiveOBB);
	}
	//投擲攻撃の当たり判定の設定
	colliderRockManager_ = std::make_unique<ColliderManager>();
	colliderRockManager_->SetRadius(2.f);
	colliderRockManager_->SetCollisionMask(kCollisionMaskEnemyAttack);
	colliderRockManager_->SetCollisionAttribute(kCollisionAttributeEnemyAttack);
	colliderRockManager_->SetCollisionPrimitive(kCollisionPrimitiveSphere);


	InitializeImpact();
	clear_ = false;
	time_ = 120;


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();
	worldTransformRock_.UpdateMatrix();

	worldTransformImpact_.UpdateMatrix();
	for (int i = 0; i < EnemyConstants::kImpactColliderCount; ++i) {
		worldTransformColliderImpact_[i].UpdateMatrix();
		colliderManager_[i]->SetWorldTransform(worldTransformColliderImpact_[i]);
	}


	emitter_.resize(5);
	accelerationFiled_.resize(5);
	particle_.resize(5);
	for (int i = 0; i < 5; ++i) {
		emitter_[i] = {
		.translate{0,1,0},
		.count{25},
		.frequency{0.075f},
		.frequencyTime{0.5f},
		.scaleRange{.min{1.f,1.f,1.f},.max{1.f,1.f,1.f}},
		.translateRange{.min{0.f,0.f,0.f},.max{0.f,0.f,0.f}},
		.colorRange{.min{0.33f,0,0.33f},.max{0.5f,0,1.0f}},
		.alphaRange{.min{1.0f},.max{1.0f}},
		.lifeTimeRange{.min{3.f},.max{3.f}},
		.velocityRange{.min{-0.6f,0.f,-0.6f},.max{0.6f,0.f,0.6f}},
		};

		accelerationFiled_[i] = {
			.acceleration{0.f,0.f,0.f},
			.translate{0.f,1.f,0.f},
			.min{-3.f,-3.f,-3.f},
			.max{3.f,3.f,3.f},
		};

		
		
		
		particle_[i] = ParticleManager::Create("resources/particle/circle.png", 15 + (i + 1));
		
	}

	AABB aabbSize{ .min{-3.0f,-2.0f,-2.0f},.max{3.0f,8.0f,2.0f} };
	OBB obb = Math::ConvertAABBToOBB(aabbSize);
	obb.center = { 0.0f,4.0f,0.0f };
	SetOBB(obb);
	SetCollisionPrimitive(kCollisionPrimitiveOBB);
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	
	animation_->Update(animationNumber_);
	animation_->SetpreAnimationTimer(0.0f);
	animation_->SetAnimationTimer(0.0f, 0.0f);
	
}

void Enemy::Update() {


	if (behaviorRequest_) {
		// 振る舞い変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			MoveInitialize();
			break;
		case Behavior::kAttack:
			AttackInitialize();
			break;
		case Behavior::kDead:
			DeadInitilize();
			break;
		}

		// 振る舞いリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		// 通常行動
		if (battleStart_) {
			MoveUpdata();
		}
		break;
	case Behavior::kAttack:
		AttackUpdata();
		break;
	case Behavior::kDead:
		DeadUpdata();
		break;
	}

	// デスフラグが立った子分を削除
	henchmans_.remove_if([](EnemyHenchman* enemy) {
		if (enemy->IsDead()) {
			
			delete enemy;
			return true;
		}
		return false;
	});

	for (EnemyHenchman* enemy : henchmans_) {
		enemy->SetRenjuPos(hmansRenjuPos_);
		enemy->SetSpacal(special_);
		enemy->Update();
	}
	
	
	barrierModel_->SetThreshold(barrierThreshold_);
	
	
	//アニメーションの更新
	animation_->Update(animationNumber_);
	//親子関係
	Relationship();
	//行列の更新
	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.TransferMatrix();
	worldTransformRock_.UpdateMatrix();
	colliderRockManager_->SetWorldTransform(worldTransformRock_);
	worldTransformImpact_.UpdateMatrix();
	worldTransformArea_.UpdateMatrix();
	worldTransformCircleArea_.UpdateMatrix();
	worldTransformBarrier_.UpdateMatrix();
	for (int i = 0; i < EnemyConstants::kImpactColliderCount; ++i) {
		worldTransformColliderImpact_[i].UpdateMatrix();
		colliderManager_[i]->SetWorldTransform(worldTransformColliderImpact_[i]);
	}


	
}

void Enemy::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformBody_, camera, true);
	if (attack_==BehaviorAttack::kGround && behavior_ == Behavior::kAttack && isAttack_ == true) {
		impactModel_->Draw(worldTransformImpact_, camera, false);
	}

	if (areaDraw_) {
		if (attack_ == BehaviorAttack::kDash) {
			areaModel_->Draw(worldTransformArea_, camera, true);
		}
		if (attack_ == BehaviorAttack::kThrowing) {
			circleAreaModel_->Draw(worldTransformCircleArea_, camera, true);
		}
	}
	
	
	for (EnemyHenchman* enemy : henchmans_) {
		enemy->Draw(camera);
	}

	for (int i = 0; i < 5; ++i) {
		particle_[i]->Draw(camera);
	}

	RenderCollisionBounds(worldTransformBody_, camera);

	
}

void Enemy::BarrierDraw(const ViewProjection& camera){
	if (barrier_ || barrierThreshold_ < 1) {
		barrierModel_->Draw(worldTransformBarrier_, camera, false);
	}
	
}

//移動
void Enemy::MoveInitialize() {
	time_ = EnemyConstants::kMoveInitTime;
	isAttack_ = false;
	aimPlayer_ = false;
	aimHealer_ = false;
	aimRenju_ = false;
	aimTank_  = false;
	behaviorAttack_ = false;
	special_ = false;
	renjuSpecial_ = false;
	barrier_ = false;
	animationNumber_ = kStandby;
	animation_->SetLoop(true);
	animation_->SetAnimationTimer(0.0f, 0.0f);
	animation_->SetpreAnimationTimer(0);
	num_ = RandomGenerator::GetRandomInt(kPlayer, kTank);
	attack_ = BehaviorAttack::kNothing;
};
void Enemy::MoveUpdata() {
	if (animationNumber_ == kThreat) {
		if (animation_->GetAnimationTimer() >= EnemyConstants::kMoveAnimationTime) {
			behaviorRequest_ = Behavior::kRoot;
			animation_->SetpreAnimationTimer(0.0f);
		}
	}

	//必殺技を打てる回数を増やす
	AddSpecialCount();

	//バリアのディゾルブを消す
	if (barrierThreshold_ <= EnemyConstants::kBarrierThresholdStart) {
		barrierThreshold_ += EnemyConstants::kBarrierThresholdIncrement;
	}

	--time_;
	if (time_ <= 0) {
		behaviorRequest_ = Behavior::kAttack;
	}
	else {

		
		// 敵の座標までの距離
		float length;
		if (num_ == kPlayer) {
			length = Math::Length(Math::Subract(playerPos_, worldTransformBase_.translate));
			sub = playerPos_ - GetWorldPosition();
		}
		else if (num_ == kHealer) {
			length = Math::Length(Math::Subract(healerPos_, worldTransformBase_.translate));
			sub = healerPos_ - GetWorldPosition();
		}
		else if (num_ == kRenju) {
			length = Math::Length(Math::Subract(renjuPos_, worldTransformBase_.translate));
			sub = renjuPos_ - GetWorldPosition();
		}
		else if (num_ == kTank) {
			length = Math::Length(Math::Subract(tankPos_, worldTransformBase_.translate));
			sub = tankPos_ - GetWorldPosition();
		}
		else {
			length = Math::Length(Math::Subract(playerPos_, worldTransformBase_.translate));
			sub = playerPos_ - GetWorldPosition();
		}
		
		if (length > EnemyConstants::kCharacterLength) {
			++moveTime_;
			if (moveTime_ > EnemyConstants::kMoveUpdateTime) {
				animationNumber_ = kRun;
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


				worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
				if (num_ == kPlayer) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, playerPos_, 0.03f);
				}
				else if (num_ == kHealer) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, healerPos_, 0.03f);
				}
				else if (num_ == kRenju) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, renjuPos_, 0.03f);
				}
				else if (num_ == kTank) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, tankPos_, 0.03f);
				}
				
			}
			
		}
		else {
			moveTime_ = 0;
			animationNumber_ = kStandby;
		}

		
	}

	
};

void Enemy::AttackInitialize() {
	//1～5の番号で攻撃の抽選
	int num = RandomGenerator::GetRandomInt(1, 5);
	if (specialCount_ >= 1 && !special_ && GetBehaviorAttack() != BehaviorAttack::kBreath && GetBehaviorAttack() != BehaviorAttack::kHenchman) {
		//7～9の番号で必殺技の抽選
		num = RandomGenerator::GetRandomInt(8, 9);//max9
	}
	
	if (num == 1) {
		attackRequest_ = BehaviorAttack::kNomal;
	}
	else if (num == 2) {
		attackRequest_ = BehaviorAttack::kDash;
	}
	else if (num == 3) {
		attackRequest_ = BehaviorAttack::kThrowing;
	}
	else if (num >= 4 && num <= 5) {
		attackRequest_ = BehaviorAttack::kGround;
	}
	else if (num == 7) {
		attackRequest_ = BehaviorAttack::kHenchman;
		
	}
	else if (num <= 8) {
		attackRequest_ = BehaviorAttack::kBreath;
	}
	
	behaviorAttack_ = true;
}
void Enemy::AttackUpdata() {

	if (attackRequest_) {
		// 振る舞い変更
		attack_ = attackRequest_.value();
		// 各振る舞いごとの初期化
		switch (attack_) {
		case BehaviorAttack::kNomal:
			NomalAttackInitialize();
			break;
		case BehaviorAttack::kDash:
			DashAttackInitialize();
			break;
		case BehaviorAttack::kThrowing:
			ThrowingAttackInitialize();
			break;
		case BehaviorAttack::kGround:
			GroundAttackInitialize();
			break; 
		case BehaviorAttack::kBreath:
			SpecialBreathInit();
			break;
		case BehaviorAttack::kHenchman:
			SpecialHenchmanInit();
			break;
		}

		// 振る舞いリセット
		attackRequest_ = std::nullopt;
	}

	switch (attack_) {
	case BehaviorAttack::kNomal:
		NomalAttackUpdata();
		break;
	case BehaviorAttack::kDash:
		DashAttackUpdata();
		break;
	case BehaviorAttack::kThrowing:
		ThrowingAttackUpdata();
		break;
	case BehaviorAttack::kGround:
		GroundAttackUpdata();
		break;
	case BehaviorAttack::kBreath:
		SpecialBreathUpdata();
		break; 
	case BehaviorAttack::kHenchman:
		SpecialHenchmanUpdata();
		break;
	case BehaviorAttack::kNothing:
		behaviorRequest_ = Behavior::kRoot;
		break;
	}
}

//殴り攻撃
void Enemy::NomalAttackInitialize() {
	SelectTarget();//ターゲットを決める
	animationNumber_ = kRun;
	behaviorAttack_ = false;
	time_ = EnemyConstants::kAttackInitTime;
}
void Enemy::NomalAttackUpdata() {
	velocity_ = { 0.0f,0.0f,1.0f };
	if (!behaviorAttack_) {
		//対象まで近づいていく
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(2.0f, velocity_); // ベース速度スケールをかける
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		velocity_ = Math::Multiply(0.35f, velocity_);// 接近速度係数をかける
		Vector3 targetPos = {};
		//番号で対象を識別
		if (num_ == kPlayer) {
			sub = playerPos_ - GetWorldPosition();
			targetPos = playerPos_;
		}
		if (num_ == kHealer) {
			sub = healerPos_ - GetWorldPosition();
			targetPos = healerPos_;
		}
		if (num_ == kRenju) {
			sub = renjuPos_ - GetWorldPosition();
			targetPos = renjuPos_;
		}
		if (num_ == kTank) {
			sub = tankPos_ - GetWorldPosition();
			targetPos = tankPos_;
		}

		worldTransformBase_.translate += velocity_;
		//目的付近まで来たら攻撃
		if (Math::isWithinRange(worldTransformBase_.translate.x, targetPos.x, 2) && Math::isWithinRange(worldTransformBase_.translate.z, targetPos.z, 2)) {
			behaviorAttack_ = true;

		}

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
	else {
		animationNumber_ = kNomalAttack;
		animation_->SetLoop(false);
		--time_;
		if (time_ <= EnemyConstants::kAttackStartTime && time_ > EnemyConstants::kAttackDuration) {
			isAttack_ = true;
		}
		if (time_ <= EnemyConstants::kAttackEndTime) {
			isAttack_ = false;
		}
		if (time_ <= 0) {
			behaviorRequest_ = Behavior::kRoot;
		}

	}

	worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);




}

//ダッシュ攻撃//近いやつに攻撃するようにする
void Enemy::DashAttackInitialize() {
	time_ = EnemyConstants::kDashAttackInitTime;
	animation_->SetLoop(false);
	animation_->SetFlameTimer(EnemyConstants::kDashAttackFlameTimer);
	SelectTarget();//ターゲットを決める
}
void Enemy::DashAttackUpdata() {
	--time_;
	if (!isAttack_ && time_ < EnemyConstants::kDashAttackAreaShowTime) {
		areaDraw_ = true;
	}

	areaPos_ = { 0.0f,0.0f,0.0f };
	if (time_ < EnemyConstants::kDashAttackRunUpEndTime && time_ > EnemyConstants::kDashAttackRunUpStartTime) {
		animationNumber_ = kRunUp;
	}

	if (time_ < EnemyConstants::kDashAttackRunUpStartTime) {
		if (!isAttack_) {
			//攻撃準備
			animation_->SetLoop(true);
			animationNumber_ = kDashAttack;
			isAttack_ = true;
			velocity_ = Math::Normalize(velocity_);
			velocity_ = Math::Multiply(2.0f, velocity_);// ダッシュ攻撃速度スケール
			Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
			velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		}
	}
	else {
		//対象の方を向く
		if (num_ == kPlayer) {
			sub = playerPos_ - GetWorldPosition();
		}
		if (num_ == kHealer) {
			sub = healerPos_ - GetWorldPosition();
		}
		if (num_ == kRenju) {
			sub = renjuPos_ - GetWorldPosition();
		}
		if (num_ == kTank) {
			sub = tankPos_ - GetWorldPosition();
		}
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


		velocity_ = { 0.0f,0.0f,EnemyConstants::kDashVeloZ};


	}

	// 目標の方向に回転
	worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
	worldTransformArea_.rotate.y = worldTransformBase_.rotate.y;

	if (!isAttack_) {
		//攻撃範囲の更新
		if (time_ > EnemyConstants::kDashAttackRunUpEndTime) {
			areaPos_.z = 1.0f;
			Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformArea_.rotate.y);
			areaPos_ = Math::Normalize(areaPos_);
			areaPos_ = Math::Multiply(22, areaPos_);
			areaPos_ = Math::TransformNormal(areaPos_, rotateMatrix);
			worldTransformArea_.translate = { worldTransformBase_.translate.x + areaPos_.x,EnemyConstants::kAreaTranslate.y,worldTransformBase_.translate.z + areaPos_.z };
		}

	}

	// 追従対象からロックオン対象へのベクトル
	if (isAttack_) {

		worldTransformBase_.translate += velocity_;

	}

	if (time_ <= 0) {
		//worldTransformRock_.translate.z = 5;
		behaviorRequest_ = Behavior::kRoot;
		areaDraw_ = false;
	}

	worldTransformBase_.translate.y = 0.0f;

}

//投擲攻撃
void Enemy::ThrowingAttackInitialize() {
	worldTransformRock_.translate = worldTransformBase_.translate;
	worldTransformRock_.translate.y = EnemyConstants::kRockInitHeight;
	worldTransformRock_.scale = EnemyConstants::kRockInitScale;
	shakeTimer_ = EnemyConstants::kShakeTimerInit;
	isAttack_ = false;
	worldTransformBody_.rotate.x = 0.0f;
	animation_->SetAnimationTimer(0.0f, 0.0f);
	animation_->SetpreAnimationTimer(0);
	animationNumber_ = kSwing;
	animation_->SetLoop(false);
	SelectTarget();//ターゲットを決める
}
void Enemy::ThrowingAttackUpdata() {
	if (!isAttack_) {


		if (num_ == kPlayer) {
			sub = playerPos_ - GetWorldPosition();
			worldTransformCircleArea_.translate = { playerPos_.x,EnemyConstants::kAreaTranslate.y,playerPos_.z };

		}
		else if (num_ == kHealer) {
			sub = healerPos_ - GetWorldPosition();
			worldTransformCircleArea_.translate = { healerPos_ .x,EnemyConstants::kAreaTranslate.y, healerPos_.z };
		
		}
		else if (num_ == kRenju) {
			sub = renjuPos_ - GetWorldPosition();
			worldTransformCircleArea_.translate = { renjuPos_.x,EnemyConstants::kAreaTranslate.y,renjuPos_.z };
			
		}
		else if (num_ == kTank) {
			sub = tankPos_ - GetWorldPosition();
			worldTransformCircleArea_.translate = { tankPos_.x,EnemyConstants::kAreaTranslate.y,tankPos_.z };
		}

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

		// 回転
		worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);


		if (worldTransformRock_.scale.x < EnemyConstants::kRockMaxScale.x) {

			randX = RandomGenerator::GetRandomFloat(-0.1f, 0.1f);
			randZ = RandomGenerator::GetRandomFloat(-0.1f, 0.1f);

			worldTransformBody_.translate += Vector3{ randX, 0.0f, randZ };

			worldTransformRock_.scale.x += EnemyConstants::kRockScaleIncrement;
			worldTransformRock_.scale.y += EnemyConstants::kRockScaleIncrement;
			worldTransformRock_.scale.z += EnemyConstants::kRockScaleIncrement;
		}
		else {
			--shakeTimer_;
			worldTransformRock_.scale = EnemyConstants::kRockMaxScale;
			areaDraw_ = true;
			if (shakeTimer_ <= 0) {
				isAttack_ = true;
				worldTransformRock_.rotate = { 0.0f, 0.0f, 0.0f };

			}
		}
	}

	if (isAttack_) {

		// 回転
		worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);

		//対象に位置に投擲
		if (num_ == kPlayer) {
			sub = playerPos_ - worldTransformRock_.GetWorldPos();
			
		}
		else if (num_ == kHealer) {
			sub = healerPos_ - worldTransformRock_.GetWorldPos();
			
		}
		else if (num_ == kRenju) {
			sub = renjuPos_ - worldTransformRock_.GetWorldPos();
			
		}
		else if (num_ == kTank) {
			sub = tankPos_ - worldTransformRock_.GetWorldPos();
			
		}

		const float kThrowSpeed = 1.0f;// 投げる速度
		Vector3 direction = sub;

		Vector3 velocity = Math::Normalize(direction) * kThrowSpeed;
		worldTransformRock_.translate += velocity;
	}

	if (worldTransformRock_.translate.y <= EnemyConstants::kRockLandingHeight && isAttack_) {
		worldTransformBody_.rotate.x = 0.0f;
		worldTransformRock_.scale = { 0.0f, 0.0f, 0.0f };
		areaDraw_ = false;
		behaviorRequest_ = Behavior::kRoot;
	}

}

//地面を殴る攻撃の衝撃波
void Enemy::InitializeImpact() {

	// 各衝撃波の初期化
	for (int i = 0; i < EnemyConstants::kImpactColliderCount; ++i) {
		if (i == 0) {
			worldTransformColliderImpact_[i].rotate = { 0.0f, EnemyConstants::kInitImpactRotationY, 0.0f };
		}
		else {
			worldTransformColliderImpact_[i].rotate.y = worldTransformColliderImpact_[i - 1].rotate.y + EnemyConstants::kImpactRotationIncrement;
		}
		worldTransformColliderImpact_[i].translate = worldTransformImpact_.translate;
		worldTransformColliderImpact_[i].scale.x = worldTransformImpact_.scale.x / EnemyConstants::kImpactScaleDivisor;
		worldTransformColliderImpact_[i].scale.z = worldTransformImpact_.scale.z / EnemyConstants::kImpactScaleDivisor;
	}


}
void Enemy::UpdataImpact() {
	// 各衝撃波の更新
	for (int i = 0; i < EnemyConstants::kImpactColliderCount; ++i) {
		worldTransformColliderImpact_[i].translate += { EnemyConstants::kImpactTranslationX[i], 0.0f, EnemyConstants::kImpactTranslationZ[i] };
		worldTransformColliderImpact_[i].scale.x = worldTransformImpact_.scale.x / EnemyConstants::kImpactScaleDivisor;
		worldTransformColliderImpact_[i].scale.z = worldTransformImpact_.scale.z / EnemyConstants::kImpactScaleDivisor;
	}


}

//地面を殴る攻撃
void Enemy::GroundAttackInitialize() {
	worldTransformImpact_.translate = worldTransformBase_.translate;
	worldTransformImpact_.translate.y = EnemyConstants::kImpactTranslationY;
	isAttack_ = false;
	animation_->SetpreAnimationTimer(0.0f);
	animation_->SetAnimationTimer(0.0f, 0.0f);
	animation_->SetLoop(false);
	InitializeImpact();
};
void Enemy::GroundAttackUpdata() {
	
	animationNumber_ = kGroundAttack;
	if (animation_->GetAnimationTimer() >= EnemyConstants::kGroundAttackAnimationTime) {
		isAttack_ = true;
		worldTransformImpact_.scale += Vector3(EnemyConstants::kGroundImpactScaleIncrement, 0.0f, EnemyConstants::kGroundImpactScaleIncrement);

		// 衝撃波の更新
		UpdataImpact();

		if (worldTransformImpact_.scale.x > EnemyConstants::kGroundImpactMaxScale) {
			worldTransformImpact_.scale = {0.0f,0.0f,0.0f};
			// 衝撃波の初期化
			InitializeImpact();
			behaviorRequest_ = Behavior::kRoot;
		}
	}

}

//ブレス必殺技
void Enemy::SpecialBreathInit() {
	--specialCount_;
	worldTransformImpact_.translate = worldTransformBase_.translate;
	animationNumber_ = kThreat;
	animation_->SetpreAnimationTimer(0.0f);
	animation_->SetLoop(false);
	moveTime_ = EnemyConstants::kSpecialBreathMoveTime;
	animation_->SetFlameTimer(100.0f);
	InitializeImpact();
}
void Enemy::SpecialBreathUpdata() {
	--moveTime_;
	//max5
	//パーティクルの更新と設定
	for (int i = 0; i < 5; ++i) {
		if (i % 2 == 0) {
			accelerationVelo_[i] = { EnemyConstants::kAccelerationValue,0.f,0.f };
		}
		else {
			accelerationVelo_[i] = { -EnemyConstants::kAccelerationValue,0.f,0.f };
		}
		filedPos_ = { 0.f,0.f,5.f };
		emitter_[i].translate = { worldTransformBase_.translate.x,1.f,worldTransformBase_.translate.z };
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(tankRotation_.y);
		accelerationVelo_[i] = Math::TransformNormal(accelerationVelo_[i], rotateMatrix);
		filedPos_ = Math::TransformNormal(filedPos_, rotateMatrix);
		accelerationFiled_[i].acceleration = accelerationVelo_[i];
		accelerationFiled_[i].translate = tankPos_ + filedPos_;

		particle_[i]->SetEmitter(emitter_[i]);
		particle_[i]->SetAccelerationFiled(accelerationFiled_[i]);
		if (isAttack_) {
			particle_[i]->Update();
		}
	}

	//既定の時間になったら攻撃開始
	if (moveTime_ <= EnemyConstants::kSpecialBreathAttackStartTime && !isAttack_) {
	
		isAttack_ = true;
		animationNumber_ = kBreathAttack;
	}

	if (moveTime_ <= 0) {
		for (int i = 0; i < 5; ++i) {
			particle_[i]->StopParticle();
		}
		behaviorRequest_ = Behavior::kRoot;
		isAttack_ = false;
	}
}

//子分を出す攻撃
void Enemy::SpecialHenchmanInit()
{
	--specialCount_;
	animationNumber_ = kThreat;
	animation_->SetpreAnimationTimer(0.0f);
	animation_->SetLoop(false);
	barrier_ = true;
	moveTime_ = EnemyConstants::kSpecialHenchmanMoveTime;
	special_ = true;
	barrierThreshold_ = EnemyConstants::kBarrierInitThreshold;
	isAttack_ = false;
	worldTransformBarrier_.rotate.y = 0.0f;
	worldTransformBarrier_.scale = EnemyConstants::kBarrierScale;
}
void Enemy::SpecialHenchmanUpdata()
{
	--moveTime_;
	
	henchman_->Update(4);
	//チャージタイム
	if (moveTime_ <= 0) {
		if (barrierThreshold_ > 0.0f ) {
			barrierThreshold_ -= 0.01f;
		}
		else {
			isAttack_ = true;
			worldTransformBarrier_.scale += EnemyConstants::kBarrierScaleIncrease;
		}

		if (worldTransformBarrier_.scale.x >= EnemyConstants::kBarrierMaxScale) {
			behaviorRequest_ = Behavior::kRoot;
		}

		
	}
	else {

		
		if(barrierThreshold_ > 0.6f){
			barrierThreshold_ -= 0.01f;//バリアを徐々に描画していく
		}
		worldTransformBarrier_.translate = worldTransformBase_.translate;
		worldTransformBarrier_.translate.y = EnemyConstants::kBarrierYTranslate;
		worldTransformBarrier_.rotate.y += EnemyConstants::kBarrierRotateSpeed;
		
		
		//敵の追加
		if (moveTime_  % EnemyConstants::kEnemySpawnInterval == 0) {
			// 敵を生成、初期化
			EnemyHenchman* newEnemy = new EnemyHenchman();
			newEnemy->Init(henchman_, Vector3{ worldTransformBase_.translate.x + RandomGenerator::GetRandomFloat(-EnemyConstants::kEnemyHenchmanSpawnRange, EnemyConstants::kEnemyHenchmanSpawnRange),EnemyConstants::kEnemyHenchmanSpawnYOffset,
				worldTransformBase_.translate.z + RandomGenerator::GetRandomFloat(-EnemyConstants::kEnemyHenchmanSpawnRange, EnemyConstants::kEnemyHenchmanSpawnRange) }
			);


			henchmans_.push_back(newEnemy);
		}	

		//弓キャラの必殺技食らったら状態遷移
		if (renjuSpecial_) {
			behaviorRequest_ = Behavior::kRoot;
		}

	}

	

}

void Enemy::DeadInitilize() {
	animationNumber_ = kDeath;
	animation_->SetAnimationTimer(0.4f, 0.f);
	animation_->SetLoop(false);
	animation_->SetEdgeColor(EnemyConstants::kEdgeColor);
}
void Enemy::DeadUpdata() {

	//バリアのディゾルブを消す
	if (barrierThreshold_ <= 1.f) {
		barrierThreshold_ += EnemyConstants::kBarrierThresholdIncreaseRate;
	}

	
	threshold_ += EnemyConstants::kThresholdIncreaseRate;
	animation_->SetThreshold(threshold_);
	if (animation_->GetAnimationTimer() >= EnemyConstants::kAnimationEndTime) {
		clear_ = true;
	}



};

void Enemy::OnCollision(Collider* collider) {

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
	worldTransformBase_.UpdateMatrix();

}

void Enemy::Relationship() {
	worldTransformBody_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformBody_.scale, worldTransformBody_.rotate, worldTransformBody_.translate),
		worldTransformBase_.matWorld_);

}

void Enemy::SelectTarget()
{
	//対象が死んでいたらもう一回抽選
	while (true) {
		num_ = RandomGenerator::GetRandomInt(kPlayer, kTank);
		if (num_ == kPlayer) {
			if (attack_ == BehaviorAttack::kNomal) {
				aimPlayer_ = true;
			}
			break;
		}
		else if (!isDeadHealer_ && num_ == kHealer) {
			if (attack_ == BehaviorAttack::kNomal) {
				aimHealer_ = true;
			}
			break;
		}
		else if (!isDeadRenju_ && num_ == kRenju) {
			if (attack_ == BehaviorAttack::kNomal) {
				aimRenju_ = true;
			}
			break;
		}
		else if (!isDeadTank_ && num_ == kTank) {
			if (attack_ == BehaviorAttack::kNomal) {
				aimTank_ = true;
			}
			break;
		}
	}
}

const Vector3 Enemy::GetWorldPosition() const {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}
void Enemy::AddSpecialCount()
{
	//体力が100で割り切れてカウントが0の時追加
	if (int(hp_) % EnemyConstants::kHpThreshold == 0 && specialCount_ == 0 && hp_ != EnemyConstants::kMaxHp && hp_ != 0) {
		++specialCount_;
	}
};
Enemy::~Enemy() {}

