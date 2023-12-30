#include "Player.h"
#include <numbers>

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
	worldTransformHead_.rotate.y = 3.14f;
}

void Player::Update() { 
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
	case Behavior::kDead:
		break;
	}

	Relationship();


	// 回転
	worldTransformBase_.rotate.y = Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);

	worldTransformBase_.UpdateMatrix();
	worldTransformHead_.TransferMatrix();
	worldTransformHammer_.TransferMatrix();
	\
	ImGui::Begin("katana");
	ImGui::SliderFloat3("pos", &worldTransformHammer_.translate.x, -3.0f, 3.0f);
	ImGui::SliderFloat3("rotate", &worldTransformHammer_.rotate.x, -3.0f, 3.0f);
	ImGui::End();
}

// 移動
void Player::MoveInitialize() { worldTransformBase_.translate.y = 0.0f; };
void Player::MoveUpdata(){
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;


	 if (KeyInput::GetInstance()->GetJoystickState(0, joyState)) {

		const float value = 0.7f;
		bool isMove = false;

		// 移動速度
		const float kCharacterSpeed = 0.2f;
		// 移動量
		velocity_ = {
		   (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		   (float)joyState.Gamepad.sThumbLY / SHRT_MAX};

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
					                         ? std::numbers::pi_v<float> - destinationAngleY_
					                         : -std::numbers::pi_v<float> - destinationAngleY_;
				}
			} else {
				destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f
				                                    : -std::numbers::pi_v<float> / 2.0f;
			}
		}

		// ジャンプ
		if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			behaviorRequest_ = Behavior::kJump;
		}

		// 攻撃
		if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
			behaviorRequest_ = Behavior::kAttack;
		}


		// ダッシュボタンを押したら
		if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			behaviorRequest_ = Behavior::kDash;
		}

	 } else {
		const float value = 0.7f;
		bool isMove_ = false;
		/*----------移動処理----------*/
		float kCharacterSpeed = 0.3f;
		// 移動量
		velocity_ = {0.0f, 0.0f, 0.0f};

		// 上下移動
		if (KeyInput::GetKey(DIK_W)) {
			velocity_.z = 1;

		} else if (KeyInput::GetKey(DIK_S)) {
			velocity_.z = -1;
		}


		// 左右移動
		if (KeyInput::GetKey(DIK_A)) {
			velocity_.x = -1;

		} else if (KeyInput::GetKey(DIK_D)) {
			velocity_.x = 1;
		}

		if (KeyInput::GetKey(DIK_A) || KeyInput::GetKey(DIK_D) || KeyInput::GetKey(DIK_W) ||
		    KeyInput::GetKey(DIK_S)) {
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
					                         ? std::numbers::pi_v<float> - destinationAngleY_
					                         : -std::numbers::pi_v<float> - destinationAngleY_;
				}
			} else {
				destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f
				                                    : -std::numbers::pi_v<float> / 2.0f;
			}
		}

		// ジャンプ
		if (KeyInput::PushKey(DIK_SPACE)) {
			behaviorRequest_ = Behavior::kJump;
		}

		//攻撃
		if (KeyInput::PushKey(DIK_E)) {
			behaviorRequest_ = Behavior::kAttack;
		}

		// ダッシュ
		if (KeyInput::PushKey(DIK_F)) {
			behaviorRequest_ = Behavior::kDash;
		}
	 }
};

// ジャンプ
void Player::JumpInitialize(){
	 worldTransformBase_.translate.y = 0.0f;
	 // ジャンプ初速
	 const float kJumpFirstSpeed = 0.6f;
	 velocity_.y = kJumpFirstSpeed;
};
void Player::JumpUpdata(){
	 // 移動
	 worldTransformBase_.translate += velocity_;
	 // 重力加速度
	 const float kGravity = 0.05f;
	 // 加速ベクトル
	 Vector3 accelerationVector = {0, -kGravity, 0};
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
}
void Player::DashUpdata() {
	 // dashTimer -= 4;

	 Vector3 velocity = {0, 0, workDash_.dashSpeed};

	 velocity = Math::TransformNormal(velocity, worldTransformBase_.matWorld_);
	 worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity);

	 // 既定の時間経過で通常行動に戻る
	 if (++workDash_.dashParameter_ >= workDash_.behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	 }
}


// 攻撃
void Player::AttackInitialize() {
	 worldTransformHammer_.rotate = {0.0f, 3.14f, 0.0f};
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
	
	 // コンボ上限に達していない
	 if (workAttack_.comboIndex < ComboNum - 1) {
		// ジョイスティックの状態取得
		if (KeyInput::GetInstance()->GetPadConnect()) {
			// 攻撃ボタンをトリガーしたら
			if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
				workAttack_.comboNext = true;
			}


		} else {
			if (KeyInput::PushKey(DIK_E)) {
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
				worldTransformHammer_.translate = {0.0f, -1.6f, 0.0f};
				worldTransformHammer_.rotate = {0.0f, 3.14f, 0.0f};

				break;
			case 1:
				worldTransformHammer_.translate = {0.0f, 0.5f, 0.0f};
				worldTransformHammer_.rotate = {0.0f, 3.14f, 1.5f};
				break;
			case 2:
				worldTransformHammer_.translate = {0.0f, 0.5f, 0.0f};
				worldTransformHammer_.rotate = {worldTransformHammer_.rotate.x, 3.14f, 1.5f};
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
}

Vector3 Player::GetWorldPosition() {
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