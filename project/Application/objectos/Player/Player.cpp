#include "Player.h"
#include <CollisionManager/CollisionConfig.h>


void Player::Initialize() {

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate = PlayerConstants::kPlayerInitPosition;
	worldTransformHammer_.Initialize();
	worldTransformHammer_.rotate = PlayerConstants::kWeaponRotate;
	worldTransformHead_.Initialize();
	worldTransformCollision_.Initialize();
	worldTransformCollision_.scale = PlayerConstants::kWeaponCollisionScale;
	worldTransformCollision_.translate = PlayerConstants::kWeaponCollisionTranslate;
	worldTransformNum_.Initialize();
	worldTransformNum_.scale = PlayerConstants::kPlayerNumScale;
	damageModel_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/character/20.png"));
	alpha_ = 0.0f;

	isOver_ = false;
	animation_ = AnimationManager::Create("resources/Player", "Atlas.png", "player.gltf");
	animationNumber_ = kStandby;
	flameTime_ = PlayerConstants::kFlameTimeDefault;
	attackType_.resize(AttackType::kAttackMax);
	for (int i = 0; i < AttackType::kAttackMax; ++i) {
		attackType_[i] = false;
	}


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformHead_.TransferMatrix();
	


	AABB aabbSize{ .min{-0.5f,-0.0f,-0.4f},.max{0.5f,1.5f,0.4f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);

	animation_->SetpreAnimationTimer(0.0f);
	animation_->SetAnimationTimer(0.0f, 0.0f);
	animation_->Update(animationNumber_);
}

void Player::Update() {

	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;

	preNoAttack_ = noAttack_;
	noAttack_ = false;

	root_ = false;

	if (behaviorRequest_) {
		// 振る舞い変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			MoveInitialize();
			break;
		case Behavior::kJump:
			JumpInitialize();
			break;
		case Behavior::kDash:
			DashInitialize();
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
		if (battleStart_ || !gameStart_) {
			MoveUpdata();
		}
		
		break;
	case Behavior::kJump:
		//ジャンプ
		JumpUpdata();
		break;
	case Behavior::kDash:
		// ジャンプ
		DashUpdata();
		break;
	case Behavior::kAttack:
		// 攻撃
		AttackUpdata();
		break;
	case Behavior::kDead:
		DeadUpdata();
		break;
	}

	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kNomal && enemy_->GetAimPlayer()) {
		if (enemy_->IsAttack()) {
			isHit_ = true;
			if (isHit_ != preHit_) {
				hp_ -= PlayerConstants::kAttackEnemyDamageNormal;
				alpha_ = PlayerConstants::kDamageAlphaInitValue;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/10.png");
			}
		}
	}

	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman) {
		if (enemy_->IsAttack()) {
			isHit_ = true;
			if (isHit_ != preHit_) {
				hp_ -= PlayerConstants::kAttackEnemyDamageHenchman;
				alpha_ = PlayerConstants::kDamageAlphaInitValue;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/50.png");
			}
		}
	}


	if (hp_ <= 0 && behavior_ != Behavior::kDead) {
		behaviorRequest_ = Behavior::kDead;
	}

	if (hp_ >= 20) {
		//barrierの中にいるか
		BarrierRange();
	}

	//ダメージの色
	damageModel_->SetColor({ 1.f,1.f,1.f,alpha_ });
	//αの変更
	if (alpha_ > 0.0f) {
		alpha_ -= PlayerConstants::kDamageAlphaDecrement;
		worldTransformNum_.translate = Math::Lerp(worldTransformNum_.translate, { numMove_ }, 0.05f);
	}

	

	// 回転
	worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
	animation_->SetFlameTimer(flameTime_);
	animation_->Update(animationNumber_);
	Relationship();
	worldTransformBase_.UpdateMatrix();
	worldTransformHead_.TransferMatrix();
	worldTransformHammer_.TransferMatrix();
	worldTransformNum_.TransferMatrix();

	ImGui::Begin("Sprite");
	ImGui::DragFloat("PlayerHp", &hp_, 1.0f);
	ImGui::End();
}

void Player::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformHead_, camera, true);
	
	RenderCollisionBounds(worldTransformHead_, camera);
	
}

void Player::NoDepthDraw(const ViewProjection& camera){
	damageModel_->Draw(worldTransformNum_, camera, false);
}

// 移動
void Player::MoveInitialize() {
	worldTransformBase_.translate.y = PlayerConstants::kPlayerInitPosition.y;
	isAttack_ = false;
	dash_ = false;
	combo_ = false;
	auto_ = false;
	animation_->SetpreAnimationTimer(0);
	flameTime_ = PlayerConstants::kFlameTimeDefault;
	animation_->SetLoop(true);
};
void Player::MoveUpdata() {
	root_ = true;

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;
	animationNumber_ = kStandby;
	bool isMove = false;

	// 移動速度
	const float kCharacterSpeed = 0.4f;

	//noAttack_ = false;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		const float value = 0.7f;
		
		// 移動量
		velocity_ = {
		   (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		   (float)joyState.Gamepad.sThumbLY / SHRT_MAX };

		if (Math::Length(velocity_) > value) {
			isMove = true;
			velocity_ = Math::Normalize(velocity_);
			velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		}

		

		
		if (Input::GetInstance()->GetPadButton(XINPUT_GAMEPAD_B) && preNoAttack_) {
			//復活時間
			revivalCount_++;


		}
		else {
			revivalCount_--;

			if (revivalCount_ <= 0) {
				revivalCount_ = 0;
			}

		}


	}
	else {
		// 移動量
		velocity_ = { 0.0f, 0.0f, 0.0f };


		// 上下移動
		if (Input::PressKey(DIK_W)) {
			velocity_.z = 1;

		}
		else if (Input::PressKey(DIK_S)) {
			velocity_.z = -1;
		}


		// 左右移動
		if (Input::PressKey(DIK_A)) {
			velocity_.x = -1;

		}
		else if (Input::PressKey(DIK_D)) {
			velocity_.x = 1;
		}

		if (Input::PressKey(DIK_A) || Input::PressKey(DIK_D) || Input::PressKey(DIK_W) ||
			Input::PressKey(DIK_S)) {
			isMove = true;
			velocity_ = Math::Normalize(velocity_);
			velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		}

	
	}

	if (isMove) {
		animationNumber_ = kRun;
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(viewProjection_->rotation_.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		// 現在の位置から移動する位置へのベクトル
		Vector3 sub = (worldTransformBase_.translate + velocity_) - GetLocalPosition();
		// 平行移動
		worldTransformBase_.translate += velocity_;
		if (sub.z != 0.0f) {
			destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

			if (sub.z < 0.0f) {
				destinationAngleY_ = (sub.x >= 0.0f)
					? std::numbers::pi_v<float> -destinationAngleY_
					: -std::numbers::pi_v<float> -destinationAngleY_;
			}
		}
		else {
			destinationAngleY_ = (sub.x >= 0.0f) ? std::numbers::pi_v<float> / 2.0f
				: -std::numbers::pi_v<float> / 2.0f;
		}
	}

	// ジャンプ
	if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)||Input::PushKey(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kJump;
	}

	// 攻撃
	if (attackType_[kNormalAttack] && !preNoAttack_) {
		auto_ = true;
	}

	// ダッシュボタンを押したら
	if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)|| Input::PushKey(DIK_F)) {
		if (gameStart_) {
			behaviorRequest_ = Behavior::kDash;
		}

	}

	if (auto_) {
		animationNumber_ = kRun;
		// 敵の座標までの距離
		float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

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

		const float kSpeed = 0.7f;
		Vector3 direction = sub;

		velocity_ = Math::Normalize(direction) * kSpeed;

		worldTransformBase_.translate += velocity_;


		if (length < PlayerConstants::kAttackRange) {
			auto_ = false;
			behaviorRequest_ = Behavior::kAttack;
		}

	}
};

// ジャンプ
void Player::JumpInitialize() {
	worldTransformBase_.translate.y = PlayerConstants::kPlayerInitPosition.y;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	velocity_.y = kJumpFirstSpeed;
	animationNumber_ = kJump;
	flameTime_ = PlayerConstants::kFlameTimeDefault;
	animation_->SetAnimationTimer(0.5f, flameTime_);
	animation_->SetLoop(false);

};
void Player::JumpUpdata() {
	root_ = true;
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
		behaviorRequest_ = Behavior::kRoot;
	}
};

// ダッシユ
void Player::DashInitialize() {
	PostEffect::GetInstance()->isRadialBlur(true);
	workDash_.dashParameter_ = 0;
	worldTransformBase_.rotate.y = destinationAngleY_;
	dash_ = true;
	animationNumber_ = kRun;
}
void Player::DashUpdata() {
	// dashTimer -= 4;
	root_ = true;
	Vector3 velocity = { 0, 0, workDash_.dashSpeed };

	velocity = Math::TransformNormal(velocity, worldTransformBase_.matWorld_);
	worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity);

	// 既定の時間経過で通常行動に戻る
	if (++workDash_.dashParameter_ >= workDash_.behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
		PostEffect::GetInstance()->isRadialBlur(false);
	}
}

// 攻撃
void Player::AttackInitialize() {
	worldTransformHammer_.translate.x = 0.0f;
	animationNumber_ = kAnimeAttack;
	animation_->SetpreAnimationTimer(0);
}
void Player::AttackUpdata() {
	
	if (animation_->GetAnimationTimer() > PlayerConstants::kAttackAnimationStartTime) {
		isAttack_ = true;
	}

	if (animation_->GetAnimationTimer() >= PlayerConstants::kAttackAnimationEndTime) {
		animationNumber_ = kStandby;
		behaviorRequest_ = Behavior::kRoot;
	}

	// ダッシュボタンを押したら
	if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		behaviorRequest_ = Behavior::kDash;
	}
	// ダッシュ
	if (Input::PushKey(DIK_F)) {
		behaviorRequest_ = Behavior::kDash;
	}
}

void Player::DeadInitilize() {
	threshold_ = 0.0f;
	isDead_ = true;
};
void Player::DeadUpdata() {
	hp_ = PlayerConstants::kDeadHP;
	animation_->SetEdgeColor(PlayerConstants::kDeadEdgeColor);
	threshold_ += PlayerConstants::kDeadThresholdIncrement;
	//animation_->SetThreshold(threshold_);
	if (threshold_ >= PlayerConstants::kDeadAnimationThreshold) {
		isOver_ = true;

	}
};

// 親子関係
void Player::Relationship() {
	worldTransformHead_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformHead_.scale, worldTransformHead_.rotate, worldTransformHead_.translate),
		worldTransformBase_.matWorld_);


	worldTransformHammer_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformHammer_.scale, worldTransformHammer_.rotate,
			worldTransformHammer_.translate),
		animation_->GetJointWorldTransform("mixamorig:RightHand").matWorld_);

	worldTransformCollision_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformCollision_.scale, worldTransformCollision_.rotate,
			worldTransformCollision_.translate),
		worldTransformHammer_.matWorld_);

	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrixNum = backToFrontMatrix * Math::Inverse(camera_.matView);
	billboardMatrixNum.m[3][0] = worldTransformNum_.translate.x;
	billboardMatrixNum.m[3][1] = worldTransformNum_.translate.y;
	billboardMatrixNum.m[3][2] = worldTransformNum_.translate.z;
	worldTransformNum_.matWorld_ = Math::MakeScaleMatrix(worldTransformNum_.scale) * billboardMatrixNum;
}

void Player::InitPos() {
	behaviorRequest_ = Behavior::kRoot;
	animationNumber_ = kStandby;
	worldTransformBase_.translate = { 3.0f,0.0f,-35.0f };
	worldTransformBase_.rotate = {0.0f,0.0f,0.0f};
	destinationAngleY_ = 0.0f;
}

// 衝突を検出したら呼び出されるコールバック関数
void Player::OnCollision(Collider* collider) {


	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		//敵に当たったらゲームスタート
		if (!gameStart_) {
			gameStart_ = true;
		}


		if (enemy_->IsAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
			const float kSpeed = 3.0f;
			velocity_ = { 0.0f, 0.0f, -kSpeed };
			velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
			//behaviorRequest_ = Behavior::knock;

			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= PlayerConstants::kAttackEnemyDamageNormal;
				alpha_ = PlayerConstants::kDamageAlphaInitValue;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/10.png");
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
		if (enemy_->IsAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kGround) {
			
			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= PlayerConstants::kAttackEnemyDamageBreath;
				alpha_ = PlayerConstants::kDamageAlphaInitValue;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/20.png");
			}

		}

		if (enemy_->IsAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kThrowing) {
			isHit_ = true;
			if (isHit_ != preHit_) {
				hp_ -= PlayerConstants::kAttackEnemyDamageNormal;
				alpha_ = PlayerConstants::kDamageAlphaInitValue;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/10.png");
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

	if (collider->GetCollisionAttribute() == kCollisionAttributeTank) {
		if (tankDead_) {
			//noAttack_ = true;
		}
		else {
			revivalCount_ = 0;

		}
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributeRenju) {
		if (renjuDead_) {
			//noAttack_ = true;
		}
		else {
			revivalCount_ = 0;
		}
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributeHealer) {
		if (healerDead_) {
			//noAttack_ = true;
		}
		else {
			revivalCount_ = 0;
		}
	}

	
}

void Player::BarrierRange(){
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
				hp_ -= PlayerConstants::kAttackEnemyDamageBreath;
				alpha_ = PlayerConstants::kDamageAlphaInitValue;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + PlayerConstants::kPlayerDamageDisplayHeight,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/20.png");
			}
		}
	}

}

const Vector3 Player::GetWorldPosition() const {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}
Vector3 Player::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.translate.x;
	worldPos.y = worldTransformBase_.translate.y;
	worldPos.z = worldTransformBase_.translate.z;
	return worldPos;
}

Player::~Player() {

}