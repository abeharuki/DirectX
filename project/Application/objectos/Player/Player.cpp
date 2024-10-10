#include "Player.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

// コンボ定数表
const std::array<Player::ConstAttack, Player::ComboNum> Player::kConstAttacks_ = {
	{// 振りかぶり、攻撃前硬直、攻撃振り時間、硬直、振りかぶりの移動の速さ,ため移動速さ,攻撃振りの移動速さ
	 {15, 10, 15, 3, -0.04f, 0.0f, 0.14f},
	 {15, 10, 15, 3, -0.04f, 0.0f, 0.2f},
	 {15, 10, 15, 30, -0.04f, 0.0f, 0.2f}}
};


void Player::Initialize() {

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate.x = 3.0f;
	worldTransformHammer_.Initialize();
	worldTransformHammer_.rotate = {-0.7f,0.0f,1.7f};
	worldTransformHead_.Initialize();
	worldTransformCollision_.Initialize();
	worldTransformCollision_.scale = { 0.1f, 1.0f, 0.1f };
	worldTransformCollision_.translate.y = 2.0f;
	worldTransformNum_.Initialize();
	worldTransformNum_.scale = { 0.5f,0.5f,0.5f };
	damageModel_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/character/20.png"));
	alpha_ = 0.0f;

	isOver_ = false;
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("resources/Player", "Atlas.png", "player.gltf"));
	animationNumber_ = standby;
	flameTime_ = 30.0f;
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
		case Behavior::knock:
			knockInitialize();
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
	case Behavior::knock:
		knockUpdata();
		break;
	case Behavior::kDead:
		DeadUpdata();

		break;
	}
	

	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kNomal && enemy_->GetAimPlayer()) {
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

	if (Input::PushKey(DIK_O)) {
		isOver_ = true;
	}

	if (hp_ <= 0) {
		behaviorRequest_ = Behavior::kDead;
	}

	damageModel_->SetColor({ 1.f,1.f,1.f,alpha_ });

	if (alpha_ > 0.0f) {
		alpha_ -= 0.08f;
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

	animation_->SetThreshold(threshold_);
	ImGui::Begin("Player");
	ImGui::SliderFloat3("pos", &worldTransformBase_.translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("NumPos", &worldTransformNum_.translate.x, -10.0f, 10.0f);
	ImGui::DragFloat3("rotate", &worldTransformNum_.rotate.x,0.1f);
	ImGui::DragFloat3("scale", &worldTransformNum_.scale.x, 0.1f);
	ImGui::Text("EnemyLength%f", length_);
	ImGui::Text("%dAnimationNumber", animationNumber_);
	ImGui::Text("%f", hp_);
	ImGui::SliderFloat("Thres", &threshold_, 0.0f, 1.0f);
	ImGui::Text("%f", animation_->GetAnimationTimer());
	ImGui::SliderFloat("flameTime", &flameTime_, 0.0f, 60.0f);
	ImGui::End();
	ImGui::Begin("Sprite");
	ImGui::DragFloat("PlayerHp", &hp_, 1.0f);
	ImGui::End();
}

void Player::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformHead_, camera, true);
	damageModel_->Draw(worldTransformNum_, camera, false);
	RenderCollisionBounds(worldTransformHead_, camera);
	
}

// 移動
void Player::MoveInitialize() {
	worldTransformBase_.translate.y = 0.0f;
	workAttack_.isAttack = false;
	dash_ = false;
	combo_ = false;
	outo_ = false;
	animation_->SetpreAnimationTimer(0);
	flameTime_ = 30.0f;
	animation_->SetLoop(true);
};
void Player::MoveUpdata() {
	root_ = true;

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;
	animationNumber_ = standby;
	//noAttack_ = false;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		const float value = 0.7f;
		bool isMove = false;

		// 移動速度
		const float kCharacterSpeed = 0.2f;
		// 移動量
		velocity_ = {
		   (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		   (float)joyState.Gamepad.sThumbLY / SHRT_MAX };

		if (Math::Length(velocity_) > value) {
			isMove = true;
			velocity_ = Math::Normalize(velocity_);
			velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		}

		if (isMove) {
			animationNumber_ = run;
			Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(viewProjection_->rotation_.y);
			velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
			// 現在の位置から移動する位置へのベクトル
			Vector3 sub = (worldTransformBase_.translate + velocity_) - GetLocalPosition();
			// 平行移動
			worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity_);
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

		if (outo_) {
			animationNumber_ = run;
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
			worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemy_->GetWorldPosition(), 0.07f);


			if (length < 5) {
				outo_ = false;
				behaviorRequest_ = Behavior::kAttack;
			}

		}
		

		// ジャンプ
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			behaviorRequest_ = Behavior::kJump;
		}

		// 攻撃
		if (attackType_[kNormalAttack] && !preNoAttack_) {
			outo_ = true;
			// 敵の座標までの距離
			float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));
			if (length < 5) {
				outo_ = false;
				behaviorRequest_ = Behavior::kAttack;
			}
		}


		// ダッシュボタンを押したら
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			behaviorRequest_ = Behavior::kDash;
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

		//const float value = 0.7f;
		bool isMove_ = false;
		/*----------移動処理----------*/
		float kCharacterSpeed = 0.3f;

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
			isMove_ = true;
			velocity_ = Math::Normalize(velocity_);
			velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		}

		if (isMove_) {
			animationNumber_ = run;
			Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(viewProjection_->rotation_.y);
			velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
			// 現在の位置から移動する位置へのベクトル
			Vector3 sub = (worldTransformBase_.translate + velocity_) - GetLocalPosition();
			// 平行移動
			worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity_);
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
		
		

		if (outo_) {
			animationNumber_ = run;
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
			worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemy_->GetWorldPosition(), 0.07f);


			if (length < 5) {
				outo_ = false;
				behaviorRequest_ = Behavior::kAttack;
			}

		}

		// ジャンプ
		if (Input::PushKey(DIK_SPACE)) {
			behaviorRequest_ = Behavior::kJump;
		}

		//攻撃
		if (attackType_[kNormalAttack]) {
			outo_ = true;
			// 敵の座標までの距離
			float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));
			if (length < 5) {
				outo_ = false;
				behaviorRequest_ = Behavior::kAttack;
			}
			
		}

		// ダッシュ
		if (Input::PushKey(DIK_F)) {
			behaviorRequest_ = Behavior::kDash;
		}


	}
};

// ジャンプ
void Player::JumpInitialize() {
	worldTransformBase_.translate.y = 0.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	velocity_.y = kJumpFirstSpeed;
	animationNumber_ = jump;
	flameTime_ = 30.0f;
	animation_->SetAnimationTimer(0.5f, flameTime_);
	animation_->SetLoop(false);
};
void Player::JumpUpdata() {
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
}
void Player::DashUpdata() {
	// dashTimer -= 4;

	Vector3 velocity = { 0, 0, workDash_.dashSpeed };

	velocity = Math::TransformNormal(velocity, worldTransformBase_.matWorld_);
	worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity);

	// 既定の時間経過で通常行動に戻る
	if (++workDash_.dashParameter_ >= workDash_.behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
		PostEffect::GetInstance()->isRadialBlur(false);
	}
}


// ノックバック
void Player::knockInitialize() {
	if (hp_ >= 1) {
		PostEffect::GetInstance()->VignetteColor({ 0.1f, 0.0f, 0.0f });
		PostEffect::GetInstance()->isVignetting(true);
	}
	PostEffect::GetInstance()->isRadialBlur(false);
	nockTime_ = 10;
	animation_->SetAnimationTimer(0, 8.0f);
	animation_->SetpreAnimationTimer(0);
	nockBack_ = true;
};
void Player::knockUpdata() {
	worldTransformBase_.translate -= velocity_;
	worldTransformBase_.translate.y = 0;
	if (--nockTime_ <= 0) {
		PostEffect::GetInstance()->isVignetting(false);
		if (hp_ > 0) {
			behaviorRequest_ = Behavior::kRoot;
		}
		else {
			behaviorRequest_ = Behavior::kDead;
		}

		nockBack_ = false;
		animation_->SetAnimationTimer(0, 8.0f);
		animation_->SetpreAnimationTimer(0);
	}



};

// 攻撃
void Player::AttackInitialize() {
	worldTransformHammer_.translate.x = 0.0f;
	workAttack_.attackParameter_ = 0;
	workAttack_.comboIndex = 0;
	workAttack_.inComboPhase = 0;
	workAttack_.comboNext = false;
	workAttack_.isFinalAttack = false;
	animationNumber_ = animeAttack;

	animation_->SetpreAnimationTimer(0);
}
void Player::AttackUpdata() {
	
	if (animation_->GetAnimationTimer() > 0.8f) {
		workAttack_.isAttack = true;
	}

	if (animation_->GetAnimationTimer() >= 1.6f) {
		animationNumber_ = standby;
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

};
void Player::DeadUpdata() {
	animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
	threshold_ += 0.004f;
	animation_->SetThreshold(threshold_);
	if (threshold_ >= 0.7f) {
		isOver_ = true;

	}
};


// 衝突を検出したら呼び出されるコールバック関数
void Player::OnCollision(const WorldTransform& worldTransform) {
	const float kSpeed = 3.0f;
	if (!nockBack_) {
		velocity_ = { 0.0f, 0.0f, kSpeed };
		velocity_ = Math::TransformNormal(velocity_, worldTransform.matWorld_);
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


void Player::OnCollision(Collider* collider) {


	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		if (isEnemyAttack_&& enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
			const float kSpeed = 3.0f;
			velocity_ = { 0.0f, 0.0f, -kSpeed };
			velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
			//behaviorRequest_ = Behavior::knock;

			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= 10;
				alpha_ = 2.0f;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
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
		if (isEnemyAttack_) {
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

	if (collider->GetCollisionAttribute() == kCollisionAttributeTank) {
		if (tankDead_) {
			noAttack_ = true;
		}
		else {
			revivalCount_ = 0;

		}
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributeRenju) {
		if (renjuDead_) {
			noAttack_ = true;
		}
		else {
			revivalCount_ = 0;
		}
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributeHealer) {
		if (healerDead_) {
			noAttack_ = true;
		}
		else {
			revivalCount_ = 0;
		}
	}


}

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