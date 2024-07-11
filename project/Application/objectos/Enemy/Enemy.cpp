#include "Enemy.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>


void Enemy::Initialize() {

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("resources/Enemy", "enemy.png", "enemy.gltf"));


	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate.z = 10.0f;
	worldTransformBase_.rotate.y = 1.57075f*2;
	worldTransformBody_.Initialize();
	worldTransformBody_.scale = { 2.0f,2.0f,2.0f };
	worldTransformRock_.Initialize();
	worldTransformRock_.scale = { 0.0f, 0.0f, 0.0f };
	worldTransformRock_.translate.z = -15000.0f;
	clear_ = false;
	time_ = 120;

	
	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();
	worldTransformRock_.UpdateMatrix();



	AABB aabbSize{ .min{-1.0f,-3.0f,-0.8f},.max{1.0f,3.0f,0.8f} };
	SetAABB(aabbSize);
	SetCenter({ 0.0f,3.0f,0.0f });
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
	//worldTransformBody_.rotate.x = 0.0f;

}

void Enemy::Update() {

	/*
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

	*/

	Relationship();
	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.TransferMatrix();
	worldTransformRock_.UpdateMatrix();
	
	

	
	ImGui::Begin("EnemyRock");
	ImGui::SliderFloat3("pos", &worldTransformRock_.translate.x, -150.0f, 150.0f);
	ImGui::End();

	ImGui::Begin("Enemy");
	animation_->AnimationDebug();
	ImGui::SliderFloat3("rotato", &worldTransformBody_.rotate.x, -2.0f, 2.0f);
	ImGui::Text("time%d", time_);
	ImGui::End();
}

void Enemy::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformBody_, camera, true);
	RenderCollisionBounds(worldTransformBody_, camera);
}

//移動
void Enemy::MoveInitialize() {
	time_ = 60 * 2;
	isAttack_ = false;
	//worldTransformBody_.rotate.x = 1.57075f;

};
void Enemy::MoveUpdata() {


	if (--time_ <= 0) {
		behaviorRequest_ = Behavior::kAttack;
	}

};

void Enemy::AttackInitialize() {

	int num = RandomGenerator::GetRandomInt(1, 2);
	if (num == 1) {
		attackRequest_ = BehaviorAttack::kDash;
	}
	else if (num == 2) {
		attackRequest_ = BehaviorAttack::kThrowing;
	}
	else if (num == 3) {

	}

}
void Enemy::AttackUpdata() {

	if (attackRequest_) {
		// 振る舞い変更
		attack_ = attackRequest_.value();
		// 各振る舞いごとの初期化
		switch (attack_) {
		case BehaviorAttack::kDash:
		default:
			DashAttackInitialize();
			break;
		case BehaviorAttack::kThrowing:
			ThrowingAttackInitialize();
			break;
		}

		// 振る舞いリセット
		attackRequest_ = std::nullopt;
	}

	switch (attack_) {
	case BehaviorAttack::kDash:
	default:
		// 通常行動
		DashAttackUpdata();
		break;
	case BehaviorAttack::kThrowing:
		ThrowingAttackUpdata();
		break;
	}
}



//ダッシュ攻撃
void Enemy::DashAttackInitialize() {
	//worldTransformBody_.rotate.x = 1.57075f;
	num_ = RandomGenerator::GetRandomInt(1, 1);;
	time_ = 60 * 2;
}
void Enemy::DashAttackUpdata() {
	time_--;
	if (time_ > 60) {
		worldTransformBase_.rotate.y += 0.5f;
	}
	else {
		isAttack_ = true;
	}
	

	// 追従対象からロックオン対象へのベクトル
	if (isAttack_) {
		// 回転
		worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_ , 0.2f);

		if (num_ == 1) {
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



			worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, playerPos_, 0.2f);
		}
		if (num_ == 2) {
			sub = healerPos_ - GetWorldPosition();
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

			// プレイヤーの座標までの距離
			float length = Math::Length(Math::Subract(tankPos_, worldTransformBase_.translate));

			worldTransformBase_.translate =
				Math::Lerp(worldTransformBase_.translate, healerPos_, 0.2f);
		}
		if (num_ == 3) {
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
			// プレイヤーの座標までの距離
			float length = Math::Length(Math::Subract(tankPos_, worldTransformBase_.translate));

			worldTransformBase_.translate =
				Math::Lerp(worldTransformBase_.translate, renjuPos_, 0.2f);
		}
		if (num_ == 4) {
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

			worldTransformBase_.translate =
				Math::Lerp(worldTransformBase_.translate, tankPos_, 0.2f);
		}
	}

	if (time_ <= 30) {
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
	
}

void Enemy::ThrowingAttackUpdata() {
	animation_->SetLoop(false);
	animation_->Update(0);
	int num = RandomGenerator::GetRandomInt(1, 1);
	

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
			if (shakeTimer_ >= 0) {
				if (worldTransformBody_.rotate.x < 2.0f) {
					worldTransformBody_.rotate.x += 0.01f;
				}

			}
			else {

				if (worldTransformBase_.rotate.x <= 1.2f) {
					isAttack_ = true;
					worldTransformRock_.rotate = { 0.0f, 0.0f, 0.0f };
					
				}
				else {
					worldTransformBody_.rotate.x -= 0.08f;
				}


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
			worldTransformRock_.translate =	Math::Lerp(worldTransformRock_.translate, healerPos_, 0.1f);
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

	if (worldTransformRock_.translate.y <= 0.6f && isAttack_) {

		if (worldTransformBody_.rotate.x >= 0.0f) {
			worldTransformBody_.rotate.x -= 0.05f;
		}
		else {
			worldTransformBody_.rotate.x = 0.0f;
			worldTransformRock_.scale = { 0.0f, 0.0f, 0.0f };
			behaviorRequest_ = Behavior::kRoot;
		}


	}

}



void Enemy::OnCollision(Collider* collider) {

}



void Enemy::DeadInitilize() {

}
void Enemy::DeadUpdata() {
	worldTransformBase_.rotate.z -= 0.01f;
	
	animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
	threshold_ += 0.004f;
	animation_->SetThreshold(threshold_);
	if (threshold_ >= 1.0f&& worldTransformBase_.rotate.z < -1.0f) {
		clear_ = true;

	}

};

void Enemy::Relationship() {
	worldTransformBody_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformBody_.scale, worldTransformBody_.rotate, worldTransformBody_.translate),
		worldTransformBase_.matWorld_);
	/*
	worldTransformRock_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformRock_.scale, worldTransformRock_.rotate, worldTransformRock_.translate),
		worldTransformBody_.matWorld_);
		*/
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