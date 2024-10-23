#include "Renju.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>


Renju::~Renju() {
	delete behaviorTree_;

};

/// <summary>
/// 初期化
/// </summary>
void Renju::Initialize() {
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Renju", "Atlas.png", "renju.gltf"));
	animationNumber_ = standby;
	flameTime_ = 30.0f;
	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate.x = -3.0f;
	worldTransformHead_.Initialize();
	bulletModel_.reset(Model::CreateModelFromObj("resources/Renju/cube.obj", "resources/Renju/Bullet.png"));

	worldTransformNum_.Initialize();
	worldTransformNum_.scale = { 0.5f,0.5f,0.5f };
	damageModel_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/character/20.png"));
	alpha_ = 0.0f;

	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].Initialize();
		worldTransformHp_[i].translate.y = 1.5f;
		worldTransformHp_[i].translate.x = (float(i) * 0.5f) - 0.5f;
		worldTransformHp_[i].scale = { 0.5f,0.5f,0.5f };
	}



	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformHead_.TransferMatrix();

	AABB aabbSize{ .min{-0.5f,-0.0f,-0.4f},.max{0.5f,1.5f,0.4f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributeRenju);
	SetCollisionMask(kCollisionMaskRenju);


	// ビヘイビアツリーの初期化
	behaviorTree_ = new BehaviorTree<Renju>(this);
	behaviorTree_->Initialize();
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Renju::Update() {
	// 状態が変わった場合にノードの初期化を行う
	if (state_ != previousState_) {
		// 状態に応じた初期化処理を呼び出す
		if (behaviorTree_) {
			behaviorTree_->NodeInitialize();
		}
		previousState_ = state_;  // 現在の状態を記録しておく
	}

	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;

	preHitPlayer_ = isHitPlayer_;
	isHitPlayer_ = false;

	if (hp_ <= 0) {
		state_ = CharacterState::Dead;
	}

	if (behaviorTree_) {
		behaviorTree_->Update();
	}

	// デスフラグが立った弾を削除
	bullets_.remove_if([](RenjuBullet* bullet) {
		if (bullet->IsDead()) {

			delete bullet;
			return true;
		}
		return false;
		});

	for (RenjuBullet* bullet : bullets_) {

		bullet->Update();
	}


	Relationship();


	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kNomal && enemy_->GetAimRenju()) {
		if (enemy_->isAttack()) {
			isHit_ = true;
			if (isHit_ != preHit_) {
				hp_ -= 10;
				alpha_ = 2.0f;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/10.png");
			}
		}
	}

	damageModel_->SetColor({ 1.f,1.f,1.f,alpha_ });

	if (alpha_ > 0.0f) {
		alpha_ -= 0.08f;
		worldTransformNum_.translate = Math::Lerp(worldTransformNum_.translate, { numMove_ }, 0.05f);
	}

	// 回転
	worldTransformBase_.rotate.y =
		Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
	animation_->SetFlameTimer(flameTime_);
	animation_->Update(animationNumber_);
	worldTransformBase_.UpdateMatrix();
	worldTransformHead_.TransferMatrix();
	worldTransformNum_.TransferMatrix();
	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].TransferMatrix();
	}

	ImGui::Begin("Sprite");
	ImGui::DragFloat("RenjuHp", &hp_, 1.0f);
	ImGui::End();

	ImGui::Begin("Renju");
	ImGui::Text("animeNunber%d", animationNumber_);
	ImGui::Text("animeTime%f", animation_->GetAnimationTimer());
	ImGui::End();
};

void Renju::Draw(const ViewProjection& camera) {

	// 弾の描画
	for (RenjuBullet* bullet : bullets_) {
		bullet->Draw(camera);
	}


	animation_->Draw(worldTransformHead_, camera, true);

	RenderCollisionBounds(worldTransformHead_, camera);

}

void Renju::NoDepthDraw(const ViewProjection& camera) {
	damageModel_->Draw(worldTransformNum_, camera, false);
}


// 移動
void Renju::MoveInitialize() {
	worldTransformBase_.translate.y = 0.0f;
	velocity_ = { 0.0f,0.0f,0.0f };
	searchTarget_ = false;
	animation_->SetpreAnimationTimer(0);
	animation_->SetLoop(true);
	flameTime_ = 30.0f;
	animationNumber_ = standby;
};
void Renju::MoveUpdate() {
	--coolTime;
	// 敵の座標までの距離
	enemylength_ = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));


	// プレイヤーに集合
	if (operation_ || !searchTarget_) {
		followPlayer_ = true;
		searchTarget_ = false;
	}
	// 敵を探す
	if (!operation_) {
		searchTarget_ = true;
		followPlayer_ = false;
		searchTarget(enemy_->GetWorldPosition());
	}

	if (isArea_ && searchTarget_ && enemy_->IsAreaDraw()) {
		const float kCharacterSpeed = 0.3f;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		worldTransformBase_.translate += velocity_;
		animationNumber_ = run;
	}
	IsVisibleToEnemy();

	//敵の攻撃が終わったらまたジャンプできるように設定
	if (!enemy_->isAttack()) {
		jumpCount_ = 1;
	}

	//地面をたたきつける攻撃が来たらジャンプする
	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kGround && enemy_->isAttack()) {
		//ジャンプは敵の攻撃一回に対して一回まで
		if (jumpCount_ == 1 && enemylength_ <= 35) {
			//敵との距離とimpactのサイズに応じてジャンプするタイミングをずらす

			if (enemylength_ < 5 && enemy_->GetImpactSize() < 10) {
				state_ = CharacterState::Jumping;
			}

			if (Math::isWithinRange(enemylength_, 10, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 20, 10)) {
				state_ = CharacterState::Jumping;
			}

			if (Math::isWithinRange(enemylength_, 20, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 40, 10)) {
				state_ = CharacterState::Jumping;
			}

			if (Math::isWithinRange(enemylength_, 30, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 60, 10)) {
				state_ = CharacterState::Jumping;
			}
		}

	}
};

// ジャンプ
void Renju::JumpInitialize() {
	--jumpCount_;
	worldTransformBase_.translate.y = 0.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	velocity_.y = kJumpFirstSpeed;

	animationNumber_ = jump;
	flameTime_ = 30.0f;
	animation_->SetAnimationTimer(0.5f, flameTime_);
	animation_->SetLoop(false);

};
void Renju::JumpUpdate() {
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
		state_ = CharacterState::Moveing;
		velocity_.y = 0.0f;

	}
};

// ノックバック
void Renju::knockInitialize() {
	nockTime_ = 10;
	animation_->SetAnimationTimer(0, 8.0f);
	animation_->SetpreAnimationTimer(0);
	nockBack_ = true;
};
void Renju::knockUpdate() {

	//worldTransformBase_.translate += velocity_;
	//worldTransformBase_.translate.y = 0;
	if (--nockTime_ <= 0) {
		nockBack_ = false;
		if (hp_ == 0) {
			state_ = CharacterState::Dead;
		}
		else {
			state_ = CharacterState::Moveing;
		}

		animation_->SetAnimationTimer(0, 8.0f);
		animation_->SetpreAnimationTimer(0);
	}
};


// 攻撃
void Renju::AttackInitialize() {
	searchTarget_ = false;
	fireTimer_ = 20;
};
void Renju::AttackUpdate() {
	// 敵の座標までの距離
	float length = Math::Length(Math::Subract(enemyPos_, worldTransformBase_.translate));

	// 距離条件チェック
	if (minDistance_ * 2 <= length && !followPlayer_) {
		state_ = CharacterState::Moveing;
		searchTarget_ = true;
	}

	if (length >= minDistance_ * 1.5f) {
		attack_ = true;
		animationNumber_ = standby;//攻撃モーションをいれたら変える
	}
	else {
		attack_ = false;
		animationNumber_ = run;
	}

	if (attack_) {
		--fireTimer_;

		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemyPos_ - GetWorldPosition();

		// y軸周りの回転
		if (sub.z != 0.0) {
			destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

			if (sub.z < 0.0) {
				destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> -destinationAngleY_
					: -std::numbers::pi_v<float> -destinationAngleY_;
			}
		}
		else {
			destinationAngleY_ =
				(sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f : -std::numbers::pi_v<float> / 2.0f;
		}

		if (fireTimer_ == 0) {
			// 弾の速度
			const float kBulletSpeed = 1.0f;

			Vector3 vector = Vector3{ enemyPos_.x,enemyPos_.y + 7,enemyPos_.z } - worldTransformBase_.GetWorldPos();
			vector = Math::Normalize(vector);
			Vector3 velocity = kBulletSpeed * vector;

			// 弾を生成、初期化
			RenjuBullet* newBullet = new RenjuBullet();
			newBullet->Initialize(
				bulletModel_.get(), worldTransformBase_.translate, { 0.3f, 0.3f, 0.3f },
				worldTransformBase_.rotate, velocity);


			bullets_.push_back(newBullet);

			fireTimer_ = 20;

			coolTime = 60;
			state_ = CharacterState::Moveing;
		}
	}
	else {
	
		const float kSpeed = 0.4f;
		velocity_ = { 0.0f, 0.0f, -kSpeed };
		velocity_ = Math::TransformNormal(velocity_, enemy_->GetWorldTransform().matWorld_);
		worldTransformBase_.translate -= velocity_;
		worldTransformBase_.translate.y = 0;


	}








	// プレイヤーに集合
	if (operation_) {
		state_ = CharacterState::Moveing;
		followPlayer_ = true;
		searchTarget_ = false;
	}
}
void Renju::UniqueInitialize() {

}

void Renju::UniqueUpdate() {

}


void Renju::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	isDead_ = true;
	animationNumber_ = death;
	animation_->SetLoop(false);
}
void Renju::DeadUpdate() {
	if (isHitPlayer_ != preHitPlayer_) {
		if (Input::GetInstance()->GetPadConnect()) {
			if (Input::GetInstance()->GetPadButton(XINPUT_GAMEPAD_B)) {
				//復活時間
				revivalCount_++;
			}
			else {
				revivalCount_--;
			}
		}
		else {
			if (Input::GetInstance()->PressKey(DIK_B)) {
				//復活時間
				revivalCount_++;
			}
			else {
				revivalCount_--;
			}
		}
	}
	else {
		revivalCount_--;
	}

	if (revivalCount_ <= 0) {
		revivalCount_ = 0;
	}

	if (revivalCount_ >= 60) {
		hp_ = 21;
		state_ = CharacterState::Moveing;
		isDead_ = false;
	}

	ImGui::Begin("revival");
	ImGui::Text("T%d", revivalCount_);
	ImGui::Text("%d", isHitPlayer_);
	ImGui::Text("%d", preHitPlayer_);
	ImGui::End();
}

void Renju::followPlayer(Vector3 playerPos) {
	if (followPlayer_) {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = playerPos - GetWorldPosition();

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
		float length = Math::Length(Math::Subract(playerPos, worldTransformBase_.translate));

		// 距離条件チェック
		if (minDistance_ <= length) {
			worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, playerPos, 0.02f);
			animationNumber_ = run;
			if (velocity_.y == 0.0f) {
				worldTransformBase_.translate.y = 0.0f;
			}

		}
		else {
			followPlayer_ = false;
			animationNumber_ = standby;
		}
	}
}

void Renju::searchTarget(Vector3 enemyPos) {
	enemyPos_ = enemyPos;
	if (!followPlayer_ && searchTarget_) {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemyPos - GetWorldPosition();

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


		// 敵の距離条件チェック
		if (minDistance_ * 2 <= enemylength_) {
			if (enemy_->GetBehaviorAttack() != BehaviorAttack::kDash || !enemy_->IsBehaberAttack()) {
				worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemyPos, 0.02f);
				animationNumber_ = run;
			}
			if (velocity_.y == 0.0f) {
				worldTransformBase_.translate.y = 0.0f;
			}
		}
		else {
			animationNumber_ = standby;
			if (coolTime <= 0 && !isArea_) {
				state_ = CharacterState::Attacking;
			}
		}
	}
}

void Renju::IsVisibleToEnemy() {
	isArea_ = false;
	//float rectWidth = 6.0f; // 横幅の設定 (敵の中心から±3)
	Vector3 toEnemy = enemyPos_ - worldTransformBase_.translate;
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
	if (enemyMinDistance_ <= distance && distance <= enemyMaxDistance_) {
		toEnemy = Math::Normalize(toEnemy); // toEnemyベクトルを正規化
		enemyForward = Math::Normalize(enemyForward); // enemyForwardベクトルを正規化

		float dot = Math::Dot(toEnemy, enemyForward);
		float angle = std::acos(dot);

		//角度条件チェック
		if (std::abs(angle) <= angleRange_) {
			if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash && enemy_->IsAreaDraw()) {
				RunAway();
				isArea_ = true;
			}

		}
	}
}

void Renju::RunAway() {
	animationNumber_ = run;
	if (enemyPos_.z > worldTransformBase_.translate.z) {
		if (enemyPos_.x > worldTransformBase_.translate.x) {
			velocity_ = { -1.0f,0.0f,-1.0f };
		}
		else {
			velocity_ = { 1.0f,0.0f,-1.0f };
		}
	}
	else {
		if (enemyPos_.x < worldTransformBase_.translate.x) {
			velocity_ = { -1.0f,0.0f,-1.0f };
		}
		else {
			velocity_ = { 1.0f,0.0f,-1.0f };
		}
	}
}

void Renju::Relationship() {
	worldTransformHead_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformHead_.scale, worldTransformHead_.rotate, worldTransformHead_.translate),
		worldTransformBase_.matWorld_);

	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrixNum = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
	billboardMatrixNum.m[3][0] = worldTransformNum_.translate.x;
	billboardMatrixNum.m[3][1] = worldTransformNum_.translate.y;
	billboardMatrixNum.m[3][2] = worldTransformNum_.translate.z;
	worldTransformNum_.matWorld_ = Math::MakeScaleMatrix(worldTransformNum_.scale) * billboardMatrixNum;
}

void Renju::InitPos(){
	worldTransformBase_.translate = { -3.0f,0.0f,-35.0f };
}

// 衝突を検出したら呼び出されるコールバック関数
void Renju::OnAllyCollision(const WorldTransform& worldTransform) {
	worldTransform;
};
void Renju::OnCollision(const WorldTransform& worldTransform) {
	worldTransform;
	//const float kSpeed = 3.0f;
	//velocity_ = { 0.0f, 0.0f, -kSpeed };
	//velocity_ = Math::TransformNormal(velocity_, worldTransform.matWorld_);
	if (hp_ > 0) {
		//behaviorRequest_ = Behavior::knock;
	}
	isHit_ = true;

	if (isHit_ != preHit_) {
		hp_ -= 10;
		alpha_ = 2.0f;
		worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
		numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
		damageModel_->SetTexture("character/10.png");
	}

};

void Renju::OnCollision(Collider* collider) {

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
			if (enemy_->isAttack()) {
				//const float kSpeed = 3.0f;
				//velocity_ = { 0.0f, 0.0f, -kSpeed };
				//velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
				if (hp_ > 0) {
					//behaviorRequest_ = Behavior::knock;
				}

				isHit_ = true;

				if (isHit_ != preHit_) {
					if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
						hp_ -= 10.0f;
						alpha_ = 2.0f;
						worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
						numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
						damageModel_->SetTexture("character/10.png");
					}
					else {
						hp_ -= 5.0f;
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

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemyAttack) {
		if (enemy_->isAttack()) {
			//const float kSpeed = 3.0f;
			//velocity_ = { 0.0f, 0.0f, -kSpeed };
			//velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
			if (hp_ > 0) {
				//behaviorRequest_ = Behavior::knock;
			}
			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= 20;
				alpha_ = 2.0f;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/20.png");
			}

		}
	}

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


	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer ||
		collider->GetCollisionAttribute() == kCollisionAttributeHealer ||
		collider->GetCollisionAttribute() == kCollisionAttributeTank) {
		const float kSpeed = 0.01f;
		float subX = collider->GetWorldTransform().matWorld_.m[3][0] - GetWorldPosition().x;
		float subZ = collider->GetWorldTransform().matWorld_.m[3][2] - GetWorldPosition().z;
		if (subX < 0) {
			allyVelocity.x = kSpeed;
		}
		else {
			allyVelocity.x = -kSpeed;
		}

		if (subZ < 0) {
			allyVelocity.z = kSpeed;
		}
		else {
			allyVelocity.z = -kSpeed;
		}

		//allyVelocity = Math::TransformNormal(allyVelocity, collider->GetWorldTransform().matWorld_);
		worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, allyVelocity);
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		isHitPlayer_ = true;
	}
}

const Vector3 Renju::GetWorldPosition() const {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

Vector3 Renju::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.translate.x;
	worldPos.y = worldTransformBase_.translate.y;
	worldPos.z = worldTransformBase_.translate.z;
	return worldPos;
}