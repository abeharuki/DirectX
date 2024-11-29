#include "Enemy.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>
#include <ParticleManager.h>


void Enemy::Initialize() {


	animation_ = AnimationManager::Create("resources/Enemy", "Atlas_Monsters.png", "Alien.gltf");
	animation2_ = AnimationManager::Create("resources/Enemy", "Atlas_Monsters.png", "Alien2.gltf");
	impactModel_.reset(Model::CreateModelFromObj("resources/Enemy/impact.obj", "resources/white.png"));
	areaModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Enemy/red_.png"));
	circleAreaModel_.reset(Model::CreateModelFromObj("resources/Enemy/area.obj", "resources/Enemy/red_.png"));
	for (int i = 0; i < 3; ++i) {
		sterModel_[i].reset(Model::CreateModelFromObj("resources/Enemy/ster.obj", "resources/Enemy/ster.png"));
		worldTransformSter_[i].Initialize();
		worldTransformSter_[i].translate.y = 12.0f;
	}
	areaModel_->DirectionalLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 2.0f, 0.0f }, 1.0f);
	//circleAreaModel_->DirectionalLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 2.0f, 0.0f }, 1.0f);
	animationNumber_ = standby;
	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate.z = 10.0f;
	worldTransformBase_.rotate.y = 1.57075f * 2;
	worldTransformBody_.Initialize();
	worldTransformBody_.scale = { 1.0f,1.0f,1.0f };
	worldTransformRock_.Initialize();
	worldTransformRock_.scale = { 0.0f, 0.0f, 0.0f };
	worldTransformRock_.translate.z = -15000.0f;
	worldTransformArea_.Initialize();
	worldTransformArea_.scale = { 4.0f,20.0f,1.0f };
	worldTransformArea_.rotate = { -1.57f,0.0f,0.0f };
	worldTransformArea_.translate.y = 0.1f;
	worldTransformCircleArea_.Initialize();
	worldTransformCircleArea_.scale = { 3.0f,3.0f,3.0f };
	worldTransformImpact_.Initialize();
	//衝撃波の当たり判定の設定
	for (int i = 0; i < 15; ++i) {
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
	for (int i = 0; i < 15; ++i) {
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
		case Behavior::kStan:
			StanInitialize();
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
	case Behavior::kStan:
		StanUpdata();
		break;
	case Behavior::kDead:
		DeadUpdata();
		break;
	}

	// デスフラグが立った弾を削除
	henchmans_.remove_if([](EnemyHenchman* enemy) {
		if (enemy->IsDead()) {
			
			delete enemy;
			return true;
		}
		return false;
	});

	for (EnemyHenchman* enemy : henchmans_) {
		enemy->SetRenjuPos(hmansRenjuPos_);
		enemy->Update();
	}
	
	for (int i = 0; i < 5; ++i) {
		//particle_[i]->Update();
	}


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
	for (int i = 0; i < 3; ++i) {
		worldTransformSter_[i].UpdateMatrix();
	}
	for (int i = 0; i < 15; ++i) {
		worldTransformColliderImpact_[i].UpdateMatrix();
		colliderManager_[i]->SetWorldTransform(worldTransformColliderImpact_[i]);
	}

	if (Input::PressKey(DIK_4)) {
		attackRequest_ = BehaviorAttack::kSpecial;
	}

	if (Input::PressKey(DIK_5)) {
		attackRequest_ = BehaviorAttack::kSpecial2;
	}

	ImGui::Begin("EnemyRock");
	ImGui::SliderFloat3("pos", &worldTransformRock_.translate.x, -150.0f, 150.0f);
	ImGui::End();

	ImGui::Begin("Enemy");
	animation_->AnimationDebug();
	ImGui::SliderFloat3("pos", &worldTransformBody_.translate.x, -2.0f, 2.0f);
	ImGui::DragFloat3("rotato", &worldTransformBody_.rotate.x, 1.0f);
	ImGui::DragFloat3("rotatoBase", &worldTransformBase_.rotate.x, 0.1f);
	ImGui::DragFloat3("Area", &worldTransformArea_.translate.x, 0.1f);
	ImGui::Text("time%d", time_);
	ImGui::Text("Attack%d", isAttack_);
	ImGui::Text("ImpactSize%f", worldTransformImpact_.scale.z);
	ImGui::Text("AnimationTime%f", animation_->GetAnimationTimer());
	ImGui::End();
}

void Enemy::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformBody_, camera, true);
	if (animationNumber_ == groundAttack && isAttack_ == true) {
		//impactModel_->Draw(worldTransformImpact_, camera, true);
	}

	if (areaDraw_) {
		if (attack_ == BehaviorAttack::kDash) {
			areaModel_->Draw(worldTransformArea_, camera, true);
		}
		if (attack_ == BehaviorAttack::kThrowing) {
			circleAreaModel_->Draw(worldTransformCircleArea_, camera, true);
		}
	}
	
	if (behavior_ == Behavior::kStan) {
		for (int i = 0; i < 3; ++i) {
			sterModel_[i]->Draw(worldTransformSter_[i], camera, true);
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

//移動
void Enemy::MoveInitialize() {
	time_ = 60;
	isAttack_ = false;
	aimPlayer_ = false;
	aimHealer_ = false;
	aimRenju_ = false;
	aimTank_  = false;
	behaviorAttack_ = false;
	animationNumber_ = standby;
	animation_->SetLoop(true);
	animation_->SetAnimationTimer(0.0f, 0.0f);
	num_ = RandomGenerator::GetRandomInt(player, tank);
};
void Enemy::MoveUpdata() {
	if (animationNumber_ == threat) {
		if (animation_->GetAnimationTimer() >= 3.0f) {
			behaviorRequest_ = Behavior::kRoot;
			animation_->SetpreAnimationTimer(0.0f);
		}
	}

	--time_;
	if (time_ <= 0) {
		behaviorRequest_ = Behavior::kAttack;
	}
	else {

		
		// 敵の座標までの距離
		float length;
		if (num_ == player) {
			length = Math::Length(Math::Subract(playerPos_, worldTransformBase_.translate));
			sub = playerPos_ - GetWorldPosition();
		}
		else if (num_ == healer) {
			length = Math::Length(Math::Subract(healerPos_, worldTransformBase_.translate));
			sub = healerPos_ - GetWorldPosition();
		}
		else if (num_ == renju) {
			length = Math::Length(Math::Subract(renjuPos_, worldTransformBase_.translate));
			sub = renjuPos_ - GetWorldPosition();
		}
		else if (num_ == tank) {
			length = Math::Length(Math::Subract(tankPos_, worldTransformBase_.translate));
			sub = tankPos_ - GetWorldPosition();
		}
		else {
			length = Math::Length(Math::Subract(playerPos_, worldTransformBase_.translate));
			sub = playerPos_ - GetWorldPosition();
		}
		
		if (length > 10) {
			++moveTime_;
			if (moveTime_ > 30) {
				animationNumber_ = run;
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
				if (num_ == player) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, playerPos_, 0.03f);
				}
				else if (num_ == healer) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, healerPos_, 0.03f);
				}
				else if (num_ == renju) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, renjuPos_, 0.03f);
				}
				else if (num_ == tank) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, tankPos_, 0.03f);
				}
				
			}
			
		}
		else {
			moveTime_ = 0;
			animationNumber_ = standby;
		}

		
	}

	/*------------------デバック用---------------------*/
	if (Input::PressKey(DIK_7)) {
		behaviorRequest_ = Behavior::kAttack;
		attackRequest_ = BehaviorAttack::kNomal;
	}
	else if (Input::PressKey(DIK_8)) {
		behaviorRequest_ = Behavior::kAttack;
		attackRequest_ = BehaviorAttack::kDash;
	}
	else if (Input::PressKey(DIK_9)) {
		behaviorRequest_ = Behavior::kAttack;
		attackRequest_ = BehaviorAttack::kThrowing;
	}
	else if (Input::PressKey(DIK_0)) {
		behaviorRequest_ = Behavior::kAttack;
		attackRequest_ = BehaviorAttack::kGround;
	}
	/*------------------------------------------------*/
};

void Enemy::AttackInitialize() {
	//1,4
	int num = RandomGenerator::GetRandomInt(1, 5);
	if (hp_ <= 400 && specialCount_ == 2 && !special_) {
		num = 7;
	}
	else if(hp_ <= 200 && specialCount_ == 1 && !special_){
		num = 7;
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
		attackRequest_ = BehaviorAttack::kSpecial;
	}
	else if (num == 8) {
		attackRequest_ = BehaviorAttack::kSpecial2;
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
		case BehaviorAttack::kSpecial:
			SpecialInitialize();
			break;
		case BehaviorAttack::kSpecial2:
			Special2Initialize();
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
	case BehaviorAttack::kSpecial:
		SpecialUpdata();
		break; 
	case BehaviorAttack::kSpecial2:
		Special2Updata();
		break;
	}
}

//殴り攻撃
void Enemy::NomalAttackInitialize() {
	//対象が死んでいたらもう一回抽選
	while (true) {
		num_ = RandomGenerator::GetRandomInt(1, 4);
		if (num_ == 1) {
			aimPlayer_ = true;
			break;
		}
		else if (!isDeadHealer_ && num_ == 2) {
			aimHealer_ = true;
			break;
		}
		else if (!isDeadRenju_ && num_ == 3) {
			aimRenju_ = true;
			break;
		}
		else if (!isDeadTank_ && num_ == 4) {
			aimTank_ = true;
			break;
		}
	}
	animationNumber_ = run;
	behaviorAttack_ = false;
	time_ = 60 * 2;
}
void Enemy::NomalAttackUpdata() {
	velocity_ = { 0.0f,0.0f,1.0f };
	if (!behaviorAttack_) {
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(2.0f, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		velocity_ = Math::Multiply(0.35f, velocity_);
		Vector3 targetPos = {};
		if (num_ == 1) {
			sub = playerPos_ - GetWorldPosition();
			targetPos = playerPos_;
		}
		if (num_ == 2) {
			sub = healerPos_ - GetWorldPosition();
			targetPos = healerPos_;
		}
		if (num_ == 3) {
			sub = renjuPos_ - GetWorldPosition();
			targetPos = renjuPos_;
		}
		if (num_ == 4) {
			sub = tankPos_ - GetWorldPosition();
			targetPos = tankPos_;
		}
		worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity_);
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
		animationNumber_ = nomalAttack;
		animation_->SetLoop(false);
		--time_;
		if (time_ <= 60 && time_ > 20) {
			isAttack_ = true;
		}
		if (time_ <= 30) {
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
	//対象が死んでいたらもう一回抽選
	while (true) {
		num_ = RandomGenerator::GetRandomInt(1, 4);
		if (num_ == 1) {
			break;
		}
		else if (!isDeadHealer_ && num_ == 2) {
			break;
		}
		else if (!isDeadRenju_ && num_ == 3) {
			break;
		}
		else if (!isDeadTank_ && num_ == 4) {
			break;
		}
	}

	time_ = 100;
	animation_->SetLoop(false);
	animation_->SetFlameTimer(40.0f);
	if (isDeadHealer_ && num_ == 2) {
		num_ = RandomGenerator::GetRandomInt(1, 4);
	}
	else if (isDeadRenju_ && num_ == 3) {
		num_ = RandomGenerator::GetRandomInt(1, 4);
	}
	else if (isDeadTank_ && num_ == 4) {
		num_ = RandomGenerator::GetRandomInt(1, 4);
	}
}
void Enemy::DashAttackUpdata() {
	--time_;
	if (!isAttack_ && time_ < 60) {
		areaDraw_ = true;
	}

	areaPos_ = { 0.0f,0.0f,0.0f };
	if (time_ < 40 && time_ > 20) {
		animationNumber_ = runUp;
	}

	if (time_ < 20) {
		if (!isAttack_) {

			animation_->SetLoop(true);
			animationNumber_ = dashAttack;
			isAttack_ = true;
			velocity_ = Math::Normalize(velocity_);
			velocity_ = Math::Multiply(2.0f, velocity_);
			Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
			velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		}
	}
	else {
		if (num_ == 1) {
			sub = playerPos_ - GetWorldPosition();
		}
		if (num_ == 2) {
			sub = healerPos_ - GetWorldPosition();
		}
		if (num_ == 3) {
			sub = renjuPos_ - GetWorldPosition();
		}
		if (num_ == 4) {
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


		velocity_ = { 0.0f,0.0f,7.0f };


	}

	// 目標の方向に回転
	worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
	worldTransformArea_.rotate.y = worldTransformBase_.rotate.y;

	if (!isAttack_) {
		if (time_ > 40) {
			areaPos_.z = 1.0f;
			Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformArea_.rotate.y);
			areaPos_ = Math::Normalize(areaPos_);
			areaPos_ = Math::Multiply(22, areaPos_);
			areaPos_ = Math::TransformNormal(areaPos_, rotateMatrix);
			worldTransformArea_.translate = { worldTransformBase_.translate.x + areaPos_.x,0.1f,worldTransformBase_.translate.z + areaPos_.z };
		}

	}

	// 追従対象からロックオン対象へのベクトル
	if (isAttack_) {

		worldTransformBase_.translate = worldTransformBase_.translate + velocity_;

	}

	if (time_ <= 0) {
		worldTransformRock_.translate.z = 5;
		behaviorRequest_ = Behavior::kRoot;
		areaDraw_ = false;
	}

	worldTransformBase_.translate.y = 0.0f;

}

//投擲攻撃
void Enemy::ThrowingAttackInitialize() {
	worldTransformRock_.translate = worldTransformBase_.translate;
	worldTransformRock_.translate.y = 18.0f;
	worldTransformRock_.scale = { 0.0f, 0.0f, 0.0f };
	shakeTimer_ = 60.0f;
	isAttack_ = false;
	worldTransformBody_.rotate.x = 0.0f;
	animation_->SetAnimationTimer(0.0f, 0.0f);
	animation_->SetpreAnimationTimer(0);
	animationNumber_ = swing;
	animation_->SetLoop(false);
	//対象が死んでいたらもう一回抽選
	while (true) {
		num_ = RandomGenerator::GetRandomInt(1, 4);
		if (num_ == 1) {
			break;
		}
		else if (!isDeadHealer_ && num_ == 2) {
			break;
		}
		else if (!isDeadRenju_ && num_ == 3) {
			break;
		}
		else if (!isDeadTank_ && num_ == 4) {
			break;
		}
	}
}
void Enemy::ThrowingAttackUpdata() {
	if (!isAttack_) {


		if (num_ == 1) {
			sub = playerPos_ - GetWorldPosition();
			worldTransformCircleArea_.translate = { playerPos_.x,0.1f,playerPos_.z };
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
		else if (num_ == 2) {
			sub = healerPos_ - GetWorldPosition();
			worldTransformCircleArea_.translate = { healerPos_ .x,0.1f, healerPos_.z };
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
		else if (num_ == 3) {
			sub = renjuPos_ - GetWorldPosition();
			worldTransformCircleArea_.translate = { renjuPos_.x,0.1f,renjuPos_.z };
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
		else if (num_ == 4) {
			sub = tankPos_ - GetWorldPosition();
			worldTransformCircleArea_.translate = { tankPos_.x,0.1f,tankPos_.z };
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

		// 回転
		worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);


		if (worldTransformRock_.scale.x < 3.0f) {

			randX = RandomGenerator::GetRandomFloat(-0.1f, 0.1f);
			randZ = RandomGenerator::GetRandomFloat(-0.1f, 0.1f);

			worldTransformBody_.translate += Vector3{ randX, 0.0f, randZ };

			worldTransformRock_.scale.x += 0.1f;
			worldTransformRock_.scale.y += 0.1f;
			worldTransformRock_.scale.z += 0.1f;
		}
		else {
			--shakeTimer_;
			worldTransformRock_.scale = { 3.0f, 3.0f, 3.0f };
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

		if (num_ == 1) {
			sub = playerPos_ - worldTransformRock_.GetWorldPos();
			worldTransformRock_.translate = Math::Lerp(worldTransformRock_.translate, playerPos_, 0.2f);
		}
		else if (num_ == 2) {
			sub = healerPos_ - worldTransformRock_.GetWorldPos();
			worldTransformRock_.translate = Math::Lerp(worldTransformRock_.translate, healerPos_, 0.2f);
		}
		else if (num_ == 3) {
			sub = renjuPos_ - worldTransformRock_.GetWorldPos();
			worldTransformRock_.translate = Math::Lerp(worldTransformRock_.translate, renjuPos_, 0.2f);
		}
		else if (num_ == 4) {
			sub = tankPos_ - worldTransformRock_.GetWorldPos();
			worldTransformRock_.translate = Math::Lerp(worldTransformRock_.translate, tankPos_, 0.2f);
		}
	}

	if (worldTransformRock_.translate.y <= 0.6f && isAttack_) {
		worldTransformBody_.rotate.x = 0.0f;
		worldTransformRock_.scale = { 0.0f, 0.0f, 0.0f };
		areaDraw_ = false;
		behaviorRequest_ = Behavior::kRoot;
	}

}

//地面を殴る攻撃の衝撃波
void Enemy::InitializeImpact() {


	for (int i = 0; i < 15; ++i) {
		if (i == 0) {
			worldTransformColliderImpact_[i].rotate = { 0.0f,0.2f,0.0f };
		}
		else {
			worldTransformColliderImpact_[i].rotate.y = worldTransformColliderImpact_[i - 1].rotate.y + 0.42f;
		}
		worldTransformColliderImpact_[i].translate = worldTransformImpact_.translate;
		worldTransformColliderImpact_[i].scale.x = worldTransformImpact_.scale.x / 100.0f;
		worldTransformColliderImpact_[i].scale.z = worldTransformImpact_.scale.z / 100.0f;
	}


}
void Enemy::UpdataImpact() {
	worldTransformColliderImpact_[0].translate += {0.12f, 0.0f, 0.6f};
	worldTransformColliderImpact_[1].translate += {0.35f, 0.0f, 0.5f};
	worldTransformColliderImpact_[2].translate += {0.53f, 0.0f, 0.31f};
	worldTransformColliderImpact_[3].translate += {0.625f, 0.0f, 0.06f};
	worldTransformColliderImpact_[4].translate += {0.6f, 0.0f, -0.2f};
	worldTransformColliderImpact_[5].translate += {0.46f, 0.0f, -0.41f};
	worldTransformColliderImpact_[6].translate += {0.25f, 0.0f, -0.56f};
	worldTransformColliderImpact_[7].translate += {-0.02f, 0.0f, -0.625f};
	worldTransformColliderImpact_[8].translate += {-0.26f, 0.0f, -0.58f};
	worldTransformColliderImpact_[9].translate += {-0.47f, 0.0f, -0.42f};
	worldTransformColliderImpact_[10].translate += {-0.6f, 0.0f, -0.2f};
	worldTransformColliderImpact_[11].translate += {-0.63f, 0.0f, 0.07f};
	worldTransformColliderImpact_[12].translate += {-0.55f, 0.0f, 0.31f};
	worldTransformColliderImpact_[13].translate += {-0.38f, 0.0f, 0.51f};
	worldTransformColliderImpact_[14].translate += {-0.15f, 0.0f, 0.61f};
	for (int i = 0; i < 15; ++i) {
		worldTransformColliderImpact_[i].scale.x = worldTransformImpact_.scale.x / 100.0f;
		worldTransformColliderImpact_[i].scale.z = worldTransformImpact_.scale.z / 100.0f;
	}


}

//地面を殴る攻撃
void Enemy::GroundAttackInitialize() {
	worldTransformImpact_.translate = worldTransformBase_.translate;
	isAttack_ = false;
	animationNumber_ = groundAttack;
	animation_->SetLoop(false);
	animation_->SetpreAnimationTimer(0.0f);
	InitializeImpact();
};
void Enemy::GroundAttackUpdata() {
	if (animation_->GetAnimationTimer() >= 1.6f) {
		isAttack_ = true;
		worldTransformImpact_.scale += Vector3(2.0f, 0.0f, 2.0f);
		UpdataImpact();
		if (worldTransformImpact_.scale.x > 100) {
			worldTransformImpact_.scale = { 1.0f,1.0f,1.0f };
			InitializeImpact();
			behaviorRequest_ = Behavior::kRoot;

		}
	}

}

//必殺技
void Enemy::SpecialInitialize() {
	--specialCount_;
	worldTransformImpact_.translate = worldTransformBase_.translate;
	animationNumber_ = groundAttack;
	animation_->SetLoop(false);
	animation_->SetpreAnimationTimer(0.0f);
	moveTime_ = 60 * 10;

	InitializeImpact();
}
void Enemy::SpecialUpdata() {
	--moveTime_;
	//max5
	for (int i = 0; i < 1; ++i) {
		if (i % 2 == 0) {
			accelerationVelo_[i] = { 10.f,0.f,0.f };
		}
		else {
			accelerationVelo_[i] = { -10.f,0.f,0.f };
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

	if (moveTime_ <= 400) {
		isAttack_ = true;
	}

	if (moveTime_ <= 0) {
		behaviorRequest_ = Behavior::kRoot;
		isAttack_ = false;
	}
}

void Enemy::Special2Initialize()
{
	--specialCount_;
	animationNumber_ = threat;
	animation_->SetLoop(false);
	animation_->SetpreAnimationTimer(0.0f);
	moveTime_ = 60*2;
}
void Enemy::Special2Updata()
{
	--moveTime_;
	if (moveTime_ <= 0) {
		behaviorRequest_ = Behavior::kRoot;
	}
	else {
		animation2_->Update(4);
		if (moveTime_ % 10 == 0) {
			// 敵を生成、初期化
			EnemyHenchman* newEnemy = new EnemyHenchman();
			newEnemy->Init(animation2_, Vector3{ worldTransformBase_.translate.x + RandomGenerator::GetRandomFloat(-10.0f, 10.0f),-1.0f,
				worldTransformBase_.translate.z + RandomGenerator::GetRandomFloat(-10.0f, 10.0f) });


			henchmans_.push_back(newEnemy);
		}	
	}
}

//スタン
void Enemy::StanInitialize() {
	sterAngle_[0] = 0.0f;
	sterAngle_[1] = 2.0f;
	sterAngle_[2] = 4.0f;
	animationNumber_ = standby;

	time_ = 60 * 3;
}
void Enemy::StanUpdata() {
	--time_;
	for (int i = 0; i < 3; ++i) {
		Math::UpdateCircularMotion3D(worldTransformSter_[i].translate.x, worldTransformSter_[i].translate.z, worldTransformBase_.translate.x, worldTransformBase_.translate.z - 2, 2.0f, sterAngle_[i], 0.1f);
	}


	if (time_ <= 0) {
		behaviorRequest_ = Behavior::kRoot;
	}
}


void Enemy::DeadInitilize() {
	animationNumber_ = death;
	animation_->SetLoop(false);
}
void Enemy::DeadUpdata() {
	

	animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
	threshold_ += 0.001f;
	animation_->SetThreshold(threshold_);
	if (animation_->GetAnimationTimer() >= 5.0f) {
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

const Vector3 Enemy::GetWorldPosition() const {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}
void Enemy::StanBehavior() { behaviorRequest_ = Behavior::kStan; };
Enemy::~Enemy() {}

