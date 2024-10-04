#include "Tank.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

Tank::~Tank() {
	delete behaviorTree_;
	
}

void Tank::Initialize() {
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/AnimatedCube", "tex.png", "bound3.gltf"));
	shield_.reset(Model::CreateModelFromObj("resources/Tank/shield.obj", "resources/white.png"));
	model_.reset(Model::CreateModelFromObj("./resources/Tank/tank.obj", "./resources/Atlas.png"));

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformHead_.rotate.y = 3.14f;
	worldTransformShield_.Initialize();
	worldTransformShield_.translate = { 0.0f,1.f,0.45f };
	worldTransformShield_.rotate.y = 3.1415f;

	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].Initialize();
		worldTransformHp_[i].translate.y = 1.5f;
		worldTransformHp_[i].translate.x = (float(i) * 0.5f) - 0.5f;
		worldTransformHp_[i].scale = { 0.5f,0.5f,0.5f };
	}

	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformHead_.TransferMatrix();

	emitter_ = {
	.translate{0,0,0},
	.count{50},
	.frequency{0.02f},
	.frequencyTime{0.0f},
	.scaleRange{.min{0.1f,0.1f,0.1f},.max{0.1f,0.1f,0.1f}},
	.translateRange{.min{-0.5f,0.0f,-0.5f},.max{0.5f,1.0f,0.5f}},
	.colorRange{.min{1.f,0,0.0f},.max{1.f,0.5f,0.0f}},
	.lifeTimeRange{.min{0.5f},.max{0.5f}},
	.velocityRange{.min{0.f,0.0f,0.f},.max{0.f,0.01f,0.0f}},
	};
	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));
	particle_->SetEmitter(emitter_);

	AABB aabbSize{ .min{-0.5f,-0.2f,-0.25f},.max{0.5f,0.2f,0.25f} };
	SetAABB(aabbSize);
	
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributeTank);
	SetCollisionMask(kCollisionMaskTank);

	// ビヘイビアツリーの初期化
	behaviorTree_ = new BehaviorTree<Tank>(this);
	behaviorTree_->Initialize();
	
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Tank::Update() {

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
		hp_ = 0;
		state_ = CharacterState::Dead;

	}

	if (behaviorTree_) {
		behaviorTree_->Update();
	}

	

	Relationship();


	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kNomal && enemy_->GetAimTank()) {
		if (enemy_->isAttack()) {
			isHit_ = true;
			if (isHit_ != preHit_) {
				hp_ -= 10;
			}
		}
	}

	// 回転
	worldTransformBase_.rotate.y =
		Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);

	worldTransformBase_.UpdateMatrix();
	worldTransformHead_.TransferMatrix();
	worldTransformShield_.TransferMatrix();
	particle_->SetTranslate(worldTransformBase_.translate);
	

	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].TransferMatrix();
	}
	if (nockBack_) {
		animation_->SetLoop(false);
		animation_->Update(0);
	}


	ImGui::Begin("Tank");
	ImGui::SliderFloat3("pos", &worldTransformBase_.translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("shieldpos", &worldTransformShield_.translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("shieldrotate", &worldTransformShield_.rotate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("enemypos", &enemy_->GetWorldTransformArea().translate.x, -10.0f, 10.0f);
	ImGui::DragFloat3("rotate", &worldTransformBase_.rotate.x);
	ImGui::Text("%d", fireTimer_);
	ImGui::Text("%f", hp_);
	ImGui::End();

	ImGui::Begin("Sprite");
	ImGui::DragFloat("TankHp", &hp_, 1.0f);
	ImGui::End();
};

void Tank::Draw(const ViewProjection& camera) {
	//animation_->Draw(worldTransformHead_, camera,true);
	model_->Draw(worldTransformHead_, camera, true);
	if (state_ == CharacterState::Unique) {
		shield_->Draw(worldTransformShield_, camera, true);
	}
	particle_->Draw(camera);
	RenderCollisionBounds(worldTransformHead_, camera);
}

// 移動
void Tank::MoveInitialize() { 
	worldTransformBase_.translate.y = 0.0f;
	velocity_ = { 0.0f,0.0f,0.0f };
	searchTarget_ = false; 
	attack_ = false;
	stanAttack_ = false;
};
void Tank::MoveUpdate() {
	--coolTime;

	// プレイヤーに集合
	if (operation_) {
		followPlayer_ = true;
		searchTarget_ = false;
	}

	// 敵を探す
	if (!operation_) {
		searchTarget_ = true;
		followPlayer_ = false;
		searchTarget(enemy_->GetWorldPosition());
	}

	if (enemy_->GetBehavior() != Behavior::kStan && !operation_) {
		if (enemy_->IsBehaberAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kNomal && mp_ >= 20) {
			state_ = CharacterState::Unique;
		}
	}
	

	if (Input::PressKey(DIK_T)) {
		state_ = CharacterState::Unique;
	}

	if (isArea_ && searchTarget_ && enemy_->IsAreaDraw()) {
		
		const float kCharacterSpeed = 0.3f;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		worldTransformBase_.translate += velocity_;
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
			
			if (Math::isWithinRange(enemylength_,10,5)&& Math::isWithinRange(enemy_->GetImpactSize(), 20, 10)) {
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
	worldTransformBase_.translate.y = 0;
};

// ジャンプ
void Tank::JumpInitialize() {
	worldTransformBase_.translate.y = 0.0f;
	--jumpCount_;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	velocity_.y = kJumpFirstSpeed;
};
void Tank::JumpUpdate() {
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
void Tank::knockInitialize() { 
	nockTime_ = 10;
	animation_->SetAnimationTimer(0, 8.0f);
	animation_->SetpreAnimationTimer(0);
	nockBack_ = true;
};
void Tank::knockUpdate() {

	//worldTransformBase_.translate += velocity_;
	//worldTransformBase_.translate.y = 0;
	if (--nockTime_ <= 0) {
		nockBack_ = false;
		if (hp_ <= 0) {
			state_ = CharacterState::Dead;
		}
		else {
			state_ = CharacterState::Moveing;
		}
		animation_->SetAnimationTimer(0, 9.0f);
		animation_->SetpreAnimationTimer(0);
	}
};


// 攻撃
void Tank::AttackInitialize() { 
	searchTarget_ = false;
	fireTimer_ = 40;
};
void Tank::AttackUpdate() {
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

	// 敵の座標までの距離
	float length = Math::Length(Math::Subract(enemyPos_, worldTransformBase_.translate));

	// 距離条件チェック
	if (minDistance_ * 2 <= length && !followPlayer_) {
		state_ = CharacterState::Moveing;
		searchTarget_ = true;
	}


	attack_ = false;
	
	if (fireTimer_ > 10) {
		velocity_ = { 0.0f,0.0f,-0.01f };
		const float kCharacterSpeed = 0.1f;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		worldTransformBase_.translate += velocity_;
	}
	else if (fireTimer_ <= 5 && fireTimer_ > 0) {
		attack_ = true;

		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemyPos_, 0.2f);
	}
	else if (fireTimer_ <= 0) {
		fireTimer_ = 40;
		coolTime = 60;
		state_ = CharacterState::Moveing;
	}


	if (!attack_) {
		//地面をたたきつける攻撃が来たらジャンプする
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kGround && enemy_->isAttack()) {
			//ジャンプは敵の攻撃一回に対して一回まで
			if (jumpCount_ == 1 && enemylength_ <= 36) {
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
	}

	// プレイヤーに集合
	if (operation_) {
		

		state_ = CharacterState::Moveing;
		followPlayer_ = true;
		searchTarget_ = false;
		attack_ = false;
	}
}


void Tank::UniqueInitialize(){
	mp_ -= 20;
	fireTimer_ = 40;
	stanAttack_ = false;
}
void Tank::UniqueUpdate(){
	--fireTimer_;
	particle_->Update();
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

	// 敵の座標までの距離
	float length = Math::Length(Math::Subract(enemyPos_, worldTransformBase_.translate));
	if (length <= 4) {
		stanAttack_ = true;
	}
	if (fireTimer_ > 10) {
		velocity_ = { 0.0f,0.0f,-0.01f };
		const float kCharacterSpeed = 0.1f;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		worldTransformBase_.translate += velocity_;
	}
	else if (fireTimer_ <= 5 && fireTimer_ > 0) {
		
		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemyPos_, 0.2f);
	}
	else if (fireTimer_ <= 0) {
		fireTimer_ = 40;
		coolTime = 60;
		stanAttack_ = false;
		state_ = CharacterState::Moveing;
	}

}


void Tank::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	isDead_ = true;
	attack_ = false;
}
void Tank::DeadUpdate() {
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

// プレイヤーに追従
void Tank::followPlayer(Vector3 playerPos) {
	playerPos_ = playerPos;
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
			worldTransformBase_.translate =
				Math::Lerp(worldTransformBase_.translate, playerPos, 0.02f);
			if (velocity_.y == 0.0f) {
				worldTransformBase_.translate.y = 0.0f;
			}
		}
		else {
			followPlayer_ = false;
		}
	}
};

// 敵を探す
void Tank::searchTarget(Vector3 enemyPos) {
	enemyPos_ = enemyPos;
	// 敵の座標までの距離
	enemylength_ = Math::Length(Math::Subract(enemyPos, worldTransformBase_.translate));
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

		// 距離条件チェック
		if (minDistance_ <= enemylength_) {
			if (enemy_->GetBehaviorAttack() != BehaviorAttack::kDash||!enemy_->IsBehaberAttack()) {
				worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemyPos, 0.02f);
			}
			
			
			if (velocity_.y == 0.0f) {
				worldTransformBase_.translate.y = 0.0f;
			}
			
		}
		else {
			if (coolTime <= 0 && !isArea_) {
				if (enemy_->GetBehaviorAttack() != BehaviorAttack::kDash) {
					state_ = CharacterState::Attacking;
				}
				else {
					if (!enemy_->IsBehaberAttack()) {
						state_ = CharacterState::Attacking;
					}
					
				}
				
			}
		

		}
	}
}

//敵の視野内かどうか
void Tank::IsVisibleToEnemy(){
	isArea_ = false;
	float rectWidth = 6.0f; // 横幅の設定 (敵の中心から±3)
	Vector3 toEnemy = enemyPos_- worldTransformBase_.translate;
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
			if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
				RunAway();
				isArea_ = true;
			}
			
		}
	}
	
}

void Tank::RunAway(){
	if (enemyPos_.z > worldTransformBase_.translate.z) {
		if (enemyPos_.x > worldTransformBase_.translate.x) {
			velocity_ = { -1.0f,0.0f,-1.2f };
		}
		else {
			velocity_ = { 1.0f,0.0f,-1.2f };
		}
	}
	else {
		if (enemyPos_.x < worldTransformBase_.translate.x) {
			velocity_ = { -1.0f,0.0f,-1.2f };
		}
		else {
			velocity_ = { 1.0f,0.0f,-1.2f };
		}
	}
}

void Tank::Relationship() {
	worldTransformHead_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformHead_.scale, worldTransformHead_.rotate, worldTransformHead_.translate),
		worldTransformBase_.matWorld_);

	worldTransformShield_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformShield_.scale, worldTransformShield_.rotate, worldTransformShield_.translate),
		worldTransformBase_.matWorld_);

	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].matWorld_ = Math::MakeScaleMatrix(worldTransformHp_[i].scale) * billboardMatrix * Math::MakeTranslateMatrix(Vector3(worldTransformBase_.translate.x + worldTransformHp_[i].translate.x, worldTransformBase_.translate.y + worldTransformHp_[i].translate.y, worldTransformBase_.translate.z));

	}
}

// 衝突を検出したら呼び出されるコールバック関数
void Tank::OnAllyCollision(const WorldTransform& worldTransform) {

};
void Tank::OnCollision(const WorldTransform& worldTransform) {
	const float kSpeed = 3.0f;
	//velocity_ = { 0.0f, 0.0f, -kSpeed };
	//velocity_ = Math::TransformNormal(velocity_, worldTransform.matWorld_);
	if (hp_ > 0) {
		//behaviorRequest_ = Behavior::knock;
	}
	

	isHit_ = true;
	if (isHit_ != preHit_) {
		hp_ -= 10.0f;

	}

};
void Tank::OnCollision(Collider* collider) {

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
			if (enemy_->isAttack()) {
				const float kSpeed = 3.0f;
				//velocity_ = { 0.0f, 0.0f, -kSpeed };
				//velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
				if (hp_ > 0) {
					//behaviorRequest_ = Behavior::knock;
				}
				isHit_ = true;

				if (isHit_ != preHit_) {
					if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
						hp_ -= 10.0f;
					}
					else {
						hp_ -= 5.0f;
					}

				}

				if (stanAttack_) {
					stanAttack_ = false;
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
			const float kSpeed = 3.0f;
			//velocity_ = { 0.0f, 0.0f, -kSpeed };
			//velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
			if (hp_ > 0) {
				//behaviorRequest_ = Behavior::knock;
			}
			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= 20.0f;
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

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		isHitPlayer_ = true;
	}

	if (
		collider->GetCollisionAttribute() == kCollisionAttributeHealer ||
		collider->GetCollisionAttribute() == kCollisionAttributeRenju) {
		const float kSpeed = 0.02f;
		float subX = collider->GetWorldTransform().matWorld_.m[3][0] - GetWorldPosition().x;
		float subZ = collider->GetWorldTransform().matWorld_.m[3][2] - GetWorldPosition().z;
		if (subX <= 0) {
			allyVelocity.x = kSpeed;
		}
		else {
			allyVelocity.x = -kSpeed;
		}

		if (subZ <= 0) {
			allyVelocity.z = kSpeed;
		}
		else {
			allyVelocity.z = -kSpeed;
		}


		worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, allyVelocity);
	}	
}

const Vector3 Tank::GetWorldPosition() const {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}
Vector3 Tank::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.translate.x;
	worldPos.y = worldTransformBase_.translate.y;
	worldPos.z = worldTransformBase_.translate.z;
	return worldPos;
}