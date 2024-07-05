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
	worldTransformBase_.translate.x = 2.0f;
	worldTransformHammer_.Initialize();
	worldTransformHammer_.rotate.y = 3.14f;
	worldTransformHead_.Initialize();
	//worldTransformHead_.rotate.y = 3.14f;
	worldTransformCollision_.Initialize();
	worldTransformCollision_.scale = { 0.1f, 1.0f, 0.1f };
	worldTransformCollision_.translate.y = 2.0f;
	a = 0.0f;
	isOver_ = false;
	hitCount_ = 2;//6;
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/AnimatedCube", "tex.png", "bound3.gltf"));


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformHead_.TransferMatrix();

	

	AABB aabbSize{ .min{-0.3f,-0.2f,-0.1f},.max{0.3f,0.2f,0.1f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
}

void Player::Update() {

	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	
	hit_ = false;

	preNoAttack_ = noAttack_;
	noAttack_ = false;

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

	Relationship();


	if (Input::PushKey(DIK_O)) {
		isOver_ = true;
	}

	

	// 回転
	worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
	
	worldTransformBase_.UpdateMatrix();
	worldTransformHead_.TransferMatrix();
	worldTransformHammer_.TransferMatrix();
	if (nockBack_) {
		animation_->SetLoop(false);
		animation_->Update(0);
	}
	
	animation_->SetThreshold(threshold_);
	ImGui::Begin("Player");
	ImGui::SliderFloat3("pos", &worldTransformBase_.translate.x, -10.0f, 10.0f);
	ImGui::Text("%d", noAttack_);
	ImGui::Text("%d", hitCount_);
	ImGui::SliderFloat("Thres", &threshold_, 0.0f, 1.0f);
	ImGui::End();
}

void Player::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformHead_, camera,true);

}

// 移動
void Player::MoveInitialize() {
	worldTransformBase_.translate.y = 0.0f;
	workAttack_.isAttack = false;
	dash_ = false;
	combo_ = false;
};
void Player::MoveUpdata() {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

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

		// ジャンプ
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			behaviorRequest_ = Behavior::kJump;
		}

		// 攻撃
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B) && !preNoAttack_) {
			behaviorRequest_ = Behavior::kAttack;
		}


		// ダッシュボタンを押したら
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			behaviorRequest_ = Behavior::kDash;
		}


		if (Input::GetInstance()->GetPadButton(XINPUT_GAMEPAD_B)&& preNoAttack_) {
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
		const float value = 0.7f;
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

		// ジャンプ
		if (Input::PushKey(DIK_SPACE)) {
			behaviorRequest_ = Behavior::kJump;
		}

		//攻撃
		if (Input::PushKey(DIK_E)) {
			behaviorRequest_ = Behavior::kAttack;
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
	}
}


// ノックバック
void Player::knockInitialize() {
	if (hitCount_ != 1) {
		PostEffect::GetInstance()->VignetteColor({ 0.1f, 0.0f, 0.0f });
		PostEffect::GetInstance()->isVignetting(true);
	}
	
	nockTime_ = 10;
	animation_->SetAnimationTimer(0, 8.0f);
	animation_->SetpreAnimationTimer(0);
	nockBack_ = true;
	hitCount_--;
};
void Player::knockUpdata() {
	worldTransformBase_.translate += velocity_;
	worldTransformBase_.translate.y = 0;
	if (--nockTime_ <= 0) {
		PostEffect::GetInstance()->isVignetting(false);
		if (hitCount_ != 0) {
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
	worldTransformHammer_.rotate = { 0.0f, 3.14f, 0.0f };
	worldTransformHammer_.translate.x = 0.0f;
	workAttack_.attackParameter_ = 0;
	workAttack_.comboIndex = 0;
	workAttack_.inComboPhase = 0;
	workAttack_.comboNext = false;
	workAttack_.isAttack = true;
	workAttack_.isFinalAttack = false;
}
void Player::AttackUpdata() {
	float speed = 1.0f;
	if (workAttack_.comboIndex >= 1) {
		combo_ = true;
	}

	// コンボ上限に達していない
	if (workAttack_.comboIndex < ComboNum - 1) {
		// ジョイスティックの状態取得
		if (Input::GetInstance()->GetPadConnect()) {
			// 攻撃ボタンをトリガーしたら
			if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
				workAttack_.comboNext = true;
			}


		}
		else {
			if (Input::PushKey(DIK_E)) {
				// コンボ有効
				workAttack_.comboNext = true;
			}
		}

	}

	// 攻撃の合計時間
	uint32_t totalTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime +
		kConstAttacks_[workAttack_.comboIndex].chargeTime +
		kConstAttacks_[workAttack_.comboIndex].swingTime +
		kConstAttacks_[workAttack_.comboIndex].recoveryTime;

	// 既定の時間経過で通常行動に戻る
	if (++workAttack_.attackParameter_ >= totalTime) {
		// コンボ継続なら次のコンボに進む
		if (workAttack_.comboNext) {
			// コンボ継続フラグをリセット
			workAttack_.comboNext = false;
			workAttack_.attackParameter_ = 0;
			workAttack_.comboIndex++;

			switch (workAttack_.comboIndex) {
			case 0:
				worldTransformHammer_.translate = { 0.0f, -1.6f, 0.0f };
				worldTransformHammer_.rotate = { 0.0f, 3.14f, 0.0f };

				break;
			case 1:
				worldTransformHammer_.translate = { 0.0f, 0.5f, 0.0f };
				worldTransformHammer_.rotate = { 0.0f, 3.14f, 1.5f };
				break;
			case 2:
				worldTransformHammer_.translate = { 0.0f, 0.5f, 0.0f };
				worldTransformHammer_.rotate = { worldTransformHammer_.rotate.x, 3.14f, 1.5f };
				break;
			}
		}
		// コンボ継続でないなら攻撃を終了してルートビヘイビアに戻る
		else {
			behaviorRequest_ = Behavior::kRoot;
			workAttack_.isAttack = false;
			workAttack_.isFinalAttack = true;
		}
	}

	uint32_t anticipationTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime;
	uint32_t chargeTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime +
		kConstAttacks_[workAttack_.comboIndex].chargeTime;
	uint32_t swingTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime +
		kConstAttacks_[workAttack_.comboIndex].chargeTime +
		kConstAttacks_[workAttack_.comboIndex].swingTime;

	// コンボ攻撃によってモーションを分岐
	switch (workAttack_.comboIndex) {
	case 0:
		if (workAttack_.attackParameter_ < anticipationTime) {
			worldTransformHammer_.rotate.x -=
				kConstAttacks_[workAttack_.comboIndex].anticipationSpeed;
		}

		if (workAttack_.attackParameter_ >= anticipationTime &&
			workAttack_.attackParameter_ < chargeTime) {
			worldTransformHammer_.rotate.x -= kConstAttacks_[workAttack_.comboIndex].chargeSpeed;
		}

		if (workAttack_.attackParameter_ >= chargeTime &&
			workAttack_.attackParameter_ < swingTime) {
			worldTransformHammer_.rotate.x -= kConstAttacks_[workAttack_.comboIndex].swingSpeed;
			workAttack_.isAttack = true;
		}

		if (workAttack_.attackParameter_ >= swingTime && workAttack_.attackParameter_ < totalTime) {
			workAttack_.isAttack = false;
		}

		break;
	case 1:
		if (workAttack_.attackParameter_ < anticipationTime) {
			worldTransformHammer_.rotate.x -=
				kConstAttacks_[workAttack_.comboIndex].anticipationSpeed;
		}

		if (workAttack_.attackParameter_ >= anticipationTime &&
			workAttack_.attackParameter_ < chargeTime) {
			worldTransformHammer_.rotate.x += kConstAttacks_[workAttack_.comboIndex].chargeSpeed;
		}

		if (workAttack_.attackParameter_ >= chargeTime &&
			workAttack_.attackParameter_ < swingTime) {
			worldTransformHammer_.rotate.x -= kConstAttacks_[workAttack_.comboIndex].swingSpeed;
			workAttack_.isAttack = true;
		}

		if (workAttack_.attackParameter_ >= swingTime && workAttack_.attackParameter_ < totalTime) {
			workAttack_.isAttack = false;
		}

		break;
	case 2:
		if (workAttack_.attackParameter_ < anticipationTime) {
			worldTransformHammer_.rotate.x +=
				kConstAttacks_[workAttack_.comboIndex].anticipationSpeed;
		}

		if (workAttack_.attackParameter_ >= anticipationTime &&
			workAttack_.attackParameter_ < chargeTime) {
			worldTransformHammer_.rotate.x += kConstAttacks_[workAttack_.comboIndex].chargeSpeed;
		}

		if (workAttack_.attackParameter_ >= chargeTime &&
			workAttack_.attackParameter_ < swingTime) {
			worldTransformHammer_.rotate.x += kConstAttacks_[workAttack_.comboIndex].swingSpeed;
			workAttack_.isAttack = true;
		}

		if (workAttack_.attackParameter_ >= swingTime && workAttack_.attackParameter_ < totalTime) {
			workAttack_.isAttack = false;
		}

		break;
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
	PostEffect::GetInstance()->isRadialBlur(false);
};
void Player::DeadUpdata() { 
	animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f});
	threshold_ += 0.004f;
	animation_->SetThreshold(threshold_);
	if (threshold_ >= 0.8f) {
		isOver_ = true;
		
	}
};



// 衝突を検出したら呼び出されるコールバック関数
void Player::OnCollision(const WorldTransform& worldTransform) {
	const float kSpeed = 3.0f;
	if (!nockBack_) {
		velocity_ = { 0.0f, 0.0f, kSpeed };
		velocity_ = Math::TransformNormal(velocity_, worldTransform.matWorld_);
		behaviorRequest_ = Behavior::knock;
	}
	

	//isHit_ = true;
	if (isHit_ != preHit_) {
		hit_ = true;

	}

};



void Player::OnCollision(Collider* collider) {

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		if (isEnemyAttack_) {
			const float kSpeed = 3.0f;
			velocity_ = { 0.0f, 0.0f, -kSpeed };
			velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
			behaviorRequest_ = Behavior::knock;

			isHit_ = true;

			if (isHit_ != preHit_) {
				hit_ = true;

			}

		}


	}

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
		worldTransformBase_.matWorld_);

	worldTransformCollision_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformCollision_.scale, worldTransformCollision_.rotate,
			worldTransformCollision_.translate),
		worldTransformHammer_.matWorld_);
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