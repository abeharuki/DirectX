#include "Enemy.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>


void Enemy::Initialize() {

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("resources/Enemy", "Atlas_Monsters.png", "Alien2.gltf"));
	impactModel_.reset(Model::CreateModelFromObj("resources/Enemy/impact.obj", "resources/white.png"));
	animationNumber_ = nomal;
	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate.z = 10.0f;
	worldTransformBase_.rotate.y = 1.57075f * 2;
	worldTransformBody_.Initialize();
	worldTransformBody_.scale = { 1.0f,1.0f,1.0f };
	worldTransformRock_.Initialize();
	worldTransformRock_.scale = { 0.0f, 0.0f, 0.0f };
	worldTransformRock_.translate.z = -15000.0f;
	worldTransformImpact_.Initialize();
	clear_ = false;
	time_ = 120;


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();
	worldTransformRock_.UpdateMatrix();

	AABB aabbSize{ .min{-3.0f,-4.0f,-2.0f},.max{3.0f,4.0f,2.0f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
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
		MoveUpdata();
		break;
	case Behavior::kAttack:
		AttackUpdata();
		break;
	case Behavior::kDead:
		DeadUpdata();
		break;
	}
	

	animation_->Update(animationNumber_);
	Relationship();
	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.TransferMatrix();
	worldTransformRock_.UpdateMatrix();
	worldTransformImpact_.UpdateMatrix();



	ImGui::Begin("EnemyRock");
	ImGui::SliderFloat3("pos", &worldTransformRock_.translate.x, -150.0f, 150.0f);
	ImGui::End();

	ImGui::Begin("Enemy");
	animation_->AnimationDebug();
	ImGui::SliderFloat3("pos", &worldTransformBody_.translate.x, -2.0f, 2.0f);
	ImGui::SliderFloat3("rotato", &worldTransformBody_.rotate.x, -2.0f, 2.0f);
	ImGui::Text("time%d", time_);
	ImGui::Text("Attack%d", isAttack_);
	ImGui::End();
}

void Enemy::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformBody_, camera, true);
	if (animationNumber_ == groundAttack && isAttack_ == true) {
		impactModel_->Draw(worldTransformImpact_, camera, true);
	}
	
	RenderCollisionBounds(worldTransformBody_, camera);
}

//移動
void Enemy::MoveInitialize() {
	time_ = 60 * 2;
	isAttack_ = false;
	behaviorAttack_ = false;
	animationNumber_ = nomal;
	animation_->SetLoop(true);
	animation_->SetAnimationTimer(0.0f, 0.0f);
};
void Enemy::MoveUpdata() {
	if (--time_ <= 0) {
		behaviorRequest_ = Behavior::kAttack;
	}
};

void Enemy::AttackInitialize() {

	int num = RandomGenerator::GetRandomInt(4, 4);
	if (num == 1) {
		attackRequest_ = BehaviorAttack::kNomal;
	}
	else if (num == 2) {
		attackRequest_ = BehaviorAttack::kDash;
	}
	else if (num == 3) {
		attackRequest_ = BehaviorAttack::kThrowing;
	}
	else if (num == 4) {
		attackRequest_ = BehaviorAttack::kGround;
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
	}
}



void Enemy::NomalAttackInitialize() {
	num_ = RandomGenerator::GetRandomInt(1, 4);//4
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
		if (isWithinRange(worldTransformBase_.translate.x, targetPos.x, 2) && isWithinRange(worldTransformBase_.translate.z, targetPos.z, 2)) {
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
	num_ = RandomGenerator::GetRandomInt(1, 4);
	time_ = 40;
	animationNumber_ = runUp;
	animation_->SetLoop(false);
	animation_->SetFlameTimer(40.0f);

}
void Enemy::DashAttackUpdata() {
	--time_;
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

	// 追従対象からロックオン対象へのベクトル
	if (isAttack_) {

		worldTransformBase_.translate = worldTransformBase_.translate + velocity_;
	}

	if (time_ <= 0) {
		worldTransformRock_.translate.z = 5;
		behaviorRequest_ = Behavior::kRoot;
	}

	worldTransformBase_.translate.y = 0.0f;

}

//投擲攻撃
void Enemy::ThrowingAttackInitialize() {
	worldTransformRock_.translate = worldTransformBase_.translate;
	worldTransformRock_.translate.y = 15.0f;
	worldTransformRock_.scale = { 0.0f, 0.0f, 0.0f };
	shakeTimer_ = 60.0f;
	isAttack_ = false;
	worldTransformBody_.rotate.x = 0.0f;
	animation_->SetAnimationTimer(0.0f, 0.0f);
	animation_->SetpreAnimationTimer(0);
	animationNumber_ = swing;
	animation_->SetLoop(false);
}
void Enemy::ThrowingAttackUpdata() {
	int num = RandomGenerator::GetRandomInt(1, 4);
	if (!isAttack_) {


		if (num == 1) {
			sub = playerPos_ - GetWorldPosition();
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
		else if (num == 2) {
			sub = healerPos_ - GetWorldPosition();;
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
		else if (num == 3) {
			sub = renjuPos_ - GetWorldPosition();
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
		else if (num == 4) {
			sub = tankPos_ - GetWorldPosition();
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


		if (worldTransformRock_.scale.x < 2.0f) {

			randX = RandomGenerator::GetRandomFloat(-0.1f, 0.1f);
			randZ = RandomGenerator::GetRandomFloat(-0.1f, 0.1f);

			worldTransformBody_.translate += Vector3{ randX, 0.0f, randZ };

			worldTransformRock_.scale.x += 0.1f;
			worldTransformRock_.scale.y += 0.1f;
			worldTransformRock_.scale.z += 0.1f;
		}
		else {
			--shakeTimer_;
			worldTransformRock_.scale = { 2.0f, 2.0f, 2.0f };
			if (shakeTimer_ <= 0) {
				isAttack_ = true;
				worldTransformRock_.rotate = { 0.0f, 0.0f, 0.0f };

			}
		}
	}

	if (isAttack_) {

		// 回転
		worldTransformBase_.rotate.y =
			Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);

		if (num == 1) {
			sub = playerPos_ - worldTransformRock_.GetWorldPos();
			worldTransformRock_.translate = Math::Lerp(worldTransformRock_.translate, playerPos_, 0.1f);
		}
		else if (num == 2) {
			sub = healerPos_ - worldTransformRock_.GetWorldPos();
			worldTransformRock_.translate = Math::Lerp(worldTransformRock_.translate, healerPos_, 0.1f);
		}
		else if (num == 3) {
			sub = renjuPos_ - worldTransformRock_.GetWorldPos();
			worldTransformRock_.translate = Math::Lerp(worldTransformRock_.translate, renjuPos_, 0.1f);

		}
		else if (num == 4) {
			sub = tankPos_ - worldTransformRock_.GetWorldPos();
			worldTransformRock_.translate = Math::Lerp(worldTransformRock_.translate, tankPos_, 0.1f);

		}
	}

	if (worldTransformRock_.translate.y <= 0.5f && isAttack_) {

		worldTransformBody_.rotate.x = 0.0f;
		worldTransformRock_.scale = { 0.0f, 0.0f, 0.0f };
		behaviorRequest_ = Behavior::kRoot;


	}

}

void Enemy::GroundAttackInitialize() {
	worldTransformImpact_.translate = worldTransformBase_.translate;
	isAttack_ = false;
	animationNumber_ = groundAttack;
	animation_->SetLoop(false);
	animation_->SetpreAnimationTimer(0.0f);
};
void Enemy::GroundAttackUpdata() {
	if(animation_->GetAnimationTimer() >= 1.6f){
		isAttack_ = true;
		worldTransformImpact_.scale += Vector3(2.0f, 0.0f, 2.0f);
		if (worldTransformImpact_.scale.x > 100) {
			worldTransformImpact_.scale = { 1.0f,1.0f,1.0f };
			behaviorRequest_ = Behavior::kRoot;
		}
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



void Enemy::DeadInitilize() {

}
void Enemy::DeadUpdata() {
	worldTransformBase_.rotate.z -= 0.01f;

	animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
	threshold_ += 0.004f;
	animation_->SetThreshold(threshold_);
	if (worldTransformBase_.rotate.z < -1.0f) {
		clear_ = true;
	}

};

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

Enemy::~Enemy() {}

