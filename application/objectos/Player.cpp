#include "Player.h"
#include "camera/LockOn.h"
#include <imgui.h>
#include <numbers>

// コンボ定数表
const std::array<Player::ConstAttack, Player::ComboNum> Player::kConstAttacks_ = {
    {// 振りかぶり、攻撃前硬直、攻撃振り時間、硬直、振りかぶりの移動の速さ,ため移動速さ,攻撃振りの移動速さ
     {15, 10, 15, 3, -0.04f, 0.0f, 0.14f},
     {15, 10, 15, 3, -0.04f, 0.0f, 0.2f},
     {15, 10, 15, 30, -0.04f, 0.0f, 0.2f}}
};

void Player::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformHammer_.Initialize();
	worldTransformWW_.Initialize();
	worldTransformHead_.rotate.y = 3.14f;
	worldTransformHead_.translate.y = -1.1f;
	worldTransformWW_.scale = {0.8f, 0.8f, 0.8f};
	worldTransformWW_.translate.y = 4.3f;

#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	globalVariables->AddItem(groupName, "DashSpeed", workDash_.dashSpeed);
	// globalVariables->AddItem(groupName, "behaviorDashTime", behaviorDashTime);

#endif
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
		case Behavior::kAttack:
			AttackInitialize();
			break;
		case Behavior::kDash:
			DashInitialize();
			break;
		case Behavior::kJump:
			JumpInitialize();
			break;
		case Behavior::kDead:

			break;
		}

		// 振る舞いリセット
		behaviorRequest_ = std::nullopt;
	}

	BaseCharacter::Update();

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		// 通常行動
		Move();
		joyMove();
		break;
	case Behavior::kAttack:
		// 攻撃
		AttackUpdata();
		break;
	case Behavior::kDash:
		DashUpdata();
		break;
	case Behavior::kJump:
		JumpUpdata();
		break;
	case Behavior::kDead:
		break;
	}

	Relationship();

	/*----------落下処理----------*/

	if (!isHit_ && !isHitFloor_ && behavior_ != Behavior::kJump) {
		IsFall();
	}

	if (worldTransformBase_.translate.y < -150) {
		fallSpeed_.y = 0.0f;
		worldTransformBase_.translate = {0.0f, 0.0f, 0.0f};
	}

	// 回転
	worldTransformBase_.rotate.y =
	    Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);

	worldTransformBase_.UpdateMatrix();
	worldTransformHead_.TransferMatrix();
	worldTransformHammer_.TransferMatrix();
	worldTransformWW_.TransferMatrix();

	ImGui::Begin("Player");
	ImGui::DragFloat4("translation", &worldTransformHead_.translate.x, 0.01f);
	ImGui::Text(
	    " X%f Y%f Z%f", worldTransformBase_.matWorld_.m[3][0],
	    worldTransformBase_.matWorld_.m[3][1], worldTransformBase_.matWorld_.m[3][2]);
	ImGui::Text(" isHit%d", isHit_);
	ImGui::Text(" isHitFloor%d", isHitFloor_);
	ImGui::End();

	ImGui::Begin("Hammer");
	ImGui::Text(
	    " X%f Y%f Z%f", worldTransformHammer_.matWorld_.m[3][0],
	    worldTransformHammer_.matWorld_.m[3][1], worldTransformHammer_.matWorld_.m[3][2]);
	ImGui::DragFloat4("translation", &worldTransformHammer_.translate.x, 0.01f);
	ImGui::DragFloat4("rotate", &worldTransformHammer_.rotate.x, 0.01f);
	ImGui::Text("attack%d", workAttack_.isAttack);
	ImGui::End();
	ApplyGlobalVariables();
}

void Player::Draw(const ViewProjection& viewprojection,bool light) {
	models_[0]->Draw(worldTransformHead_, viewprojection,light);
	if (behavior_ == Behavior::kAttack) {
		models_[1]->Draw(worldTransformHammer_, viewprojection,true);
	}
}

Player::Player() {
	fallSpeed_ = {0.0f, 0.0f, 0.0f};
	isHit_ = false;
	isHitFloor_ = false;
};
Player::~Player(){};

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

Vector3 Player::GetHammerWorldPos() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformWW_.matWorld_.m[3][0];
	worldPos.y = worldTransformWW_.matWorld_.m[3][1];
	worldPos.z = worldTransformWW_.matWorld_.m[3][2];
	return worldPos;
}

void Player::SetPosition(Vector3 Pos) { worldTransformBase_.translate = Pos; }

// 移動初期化
void Player::MoveInitialize() { worldTransformBase_.translate.y = 0.0f; }
void Player::joyMove() {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;
	// ジョイスティックの状態取得
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
		}

		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);

		Matrix4x4 rotateMatrix = Math::Multiply(
		    Math::MakeRotateXMatrix(viewProjection_->rotation_.x),
		    Math::Multiply(
		        Math::MakeRotateYMatrix(viewProjection_->rotation_.y),
		        Math::MakeRotateZMatrix(viewProjection_->rotation_.z)));
		// move = utility_->Normalize(move);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);

		if (isMove) {

			worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity_);
			destinationAngleY_ = std::atan2(velocity_.x, velocity_.z);
		} else if (lockOn_ && lockOn_->ExistTarget()) {
			// ロックオン座標
			Vector3 lockOnPos = lockOn_->GetTargetPos();
			// 追従対象からロックオン対象へのベクトル
			Vector3 sub = lockOnPos - GetWorldPosition();

			// y軸周りの回転
			destinationAngleY_ = std::atan2(sub.x, sub.z);
		}
	}

	// ジャンプ
	if (isHit_ || isHitFloor_) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			behaviorRequest_ = Behavior::kJump;
		}
	}
	// 攻撃
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		behaviorRequest_ = Behavior::kAttack;
	}

	// ダッシュボタンを押したら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		behaviorRequest_ = Behavior::kDash;
	}
}
void Player::Move() {
	const float value = 0.7f;
	bool isMove_ = false;
	/*----------移動処理----------*/
	float kCharacterSpeed = 0.3f;
	// 移動量
	velocity_ = {0.0f, 0.0f, 0.0f};

	// 左右移動
	if (KeyInput::GetKey(DIK_A)) {
		velocity_.x = -1;

	} else if (KeyInput::GetKey(DIK_D)) {
		velocity_.x = 1;
	}

	// 上下移動
	if (KeyInput::GetKey(DIK_S)) {
		velocity_.z = -1;

	} else if (KeyInput::GetKey(DIK_W)) {
		velocity_.z = 1;
	}

	if (KeyInput::GetKey(DIK_W) || KeyInput::GetKey(DIK_A) || KeyInput::GetKey(DIK_S) ||
	    KeyInput::GetKey(DIK_D)) {
		isMove_ = true;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
	}

	
	if (isMove_) {
		// 移動ベクトルをカメラの角度だけ回転する
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(viewProjection_->rotation_.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		// 現在の位置から移動する位置へのベクトル
		Vector3 sub = (worldTransformBase_.translate+velocity_) - GetLocalPosition();
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
		//Rotate(velocity_);
	} else if (lockOn_ && lockOn_->ExistTarget()) {
		// ロックオン座標
		Vector3 lockOnPos = lockOn_->GetTargetPos();
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = lockOnPos - GetWorldPosition();

		// y軸周りの回転
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
	if (isHit_ || isHitFloor_) {
		if (KeyInput::GetKey(DIK_SPACE)) {
			behaviorRequest_ = Behavior::kJump;
		}
	}

	// 攻撃
	if (KeyInput::PushKey(DIK_E)) {
		behaviorRequest_ = Behavior::kAttack;
	}
	// ダッシュ
	if (KeyInput::PushKey(DIK_Q)) {
		behaviorRequest_ = Behavior::kDash;
	}
}

// ジャンプ初期化
void Player::JumpInitialize() {
	worldTransformBase_.translate.y = 0.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 1.0f;
	velocity_.y = kJumpFirstSpeed;
}
void Player::JumpUpdata() {
	// 移動
	worldTransformBase_.translate += velocity_;
	// 重力加速度
	const float kGravity = 0.05f;
	// 加速ベクトル
	Vector3 accelerationVector = {0, -kGravity, 0};
	// 加速
	velocity_ += accelerationVector;

	if (worldTransformBase_.translate.y <= 0.0f) {
		if (isHit_ || isHitFloor_) {
			// ジャンプ終了
			behaviorRequest_ = Behavior::kRoot;
		}
	}
}

// 攻撃初期化
void Player::AttackInitialize() {
	worldTransformHammer_.rotate = {0.0f, 0.0f, 0.0f};

	workAttack_.attackParameter_ = 0;
	workAttack_.comboIndex = 0;
	workAttack_.inComboPhase = 0;
	workAttack_.comboNext = false;
	workAttack_.isAttack = true;
	workAttack_.isFinalAttack = false;
}
void Player::AttackUpdata() {
	// ゲームパッドの状態を得る変数(XINPUT)
	// XINPUT_STATE joyState;

	float speed = 1.0f;
	// worldTransformHammer_.rotate.x += attackSpeed;

	// ロックオン中
	if (lockOn_ && lockOn_->ExistTarget()) {
		// ロックオン座標
		Vector3 lockOnPos = lockOn_->GetTargetPos();
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = lockOnPos - GetWorldPosition();

		// 距離
		float distance = Math::Length(sub);
		// 距離閾値
		const float threshold = 0.2f;

		if (distance > threshold) {
			// y軸周りの回転
			//destinationAngleY_ = std::atan2(sub.x, sub.z);
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
	}

	// コンボ上限に達していない
	if (workAttack_.comboIndex < ComboNum - 1) {
		// ジョイスティックの状態取得
		// if (KeyInput::GetInstance()->GetJoystickState(0, joyState)) {
		//	 // 攻撃ボタンをトリガーしたら
		//	 if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		//		 // コンボ有効
		//		 workAttack_.comboNext = true;
		//	 }
		//}
		if (KeyInput::PushKey(DIK_E)) {
			// コンボ有効
			workAttack_.comboNext = true;
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
				worldTransformHammer_.translate = {0.0f, 0.8f, 0.0f};
				worldTransformHammer_.rotate = {0.0f, 0.0f, 0.0f};

				break;
			case 1:
				worldTransformHammer_.translate = {0.0f, 0.8f, 0.0f};
				worldTransformHammer_.rotate = {1.0f, 0.0f, 3.14f / 2.0f};
				break;
			case 2:
				worldTransformHammer_.translate = {0.0f, 0.8f, 0.0f};
				worldTransformHammer_.rotate = {worldTransformHammer_.rotate.x, 0.0f, 3.14f / 2.0f};
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
	case 1:
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
	case 2:
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
	}
}

// ダッシュ初期化
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

// 移動床とのペアレント設定
void Player::Relationship(const WorldTransform& worldTransformFloor) {

	if (!worldTransformBase_.parent_) {

		// 移動床のワールド座標
		Vector3 objectWorldPos = {
		    worldTransformFloor.matWorld_.m[3][0], worldTransformFloor.matWorld_.m[3][1],
		    worldTransformFloor.matWorld_.m[3][2]};

		// プレイヤーローカル座標
		Matrix4x4 localPos = Math::Multiply(
		    Math::Inverse(worldTransformFloor.matWorld_), worldTransformBase_.matWorld_);

		Vector3 playerLocalPos = {localPos.m[3][0], localPos.m[3][1] + 1.0f, localPos.m[3][2]};

		// ローカル座標系に入れる
		// worldTransformBase_.matWorld_ = localPos;
		worldTransformBase_.translate = playerLocalPos;

		Setparent(&worldTransformFloor);
	}
}
void Player::Setparent(const WorldTransform* parent) { worldTransformBase_.parent_ = parent; };
void Player::DeleteParent() {
	if (worldTransformBase_.parent_) {
		worldTransformBase_.translate = worldTransformBase_.GetWorldPos();
		worldTransformBase_.parent_ = nullptr;
	}
}

// 落下
void Player::IsFall() {
	// 加速
	worldTransformBase_.translate += fallSpeed_;
	const float kGravity = 0.2f;
	// 加速ベクトル
	Vector3 accelerationVector = {0, -kGravity, 0};
	fallSpeed_ += accelerationVector;
}

// 当たり判定
void Player::OnCollision() { isHit_ = true; }
void Player::OutCollision() { isHit_ = false; }
void Player::OnCollisionFloor() { isHitFloor_ = true; }
void Player::OutCollisionFloor() { isHitFloor_ = false; }

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

	worldTransformWW_.matWorld_ = Math::Multiply(
	    Math::MakeAffineMatrix(
	        worldTransformWW_.scale, worldTransformWW_.rotate, worldTransformWW_.translate),
	    worldTransformHammer_.matWorld_);
}

void Player::ApplyGlobalVariables() {
#ifdef _DEBUG

	GlobalVariables* globalVaribles = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	workDash_.dashSpeed = globalVaribles->GetFloatValue(groupName, "DashSpeed");

#endif
}

void Player::Rotate(const Vector3& v) {
	Vector3 cross = Math::Normalize(Math::Cross({0.0f, 0.0f, 1.0f}, v));
	float dot = Math::Dot({0.0f, 0.0f, 1.0f}, v);
	Quaternion destinationQuaternion = Math::MakeQuaternion(cross, std::acos(dot));
	Quaternion posQuaternion = {
	    0, worldTransformBase_.translate.x, worldTransformBase_.translate.y,
	    worldTransformBase_.translate.z};

	Quaternion posQuaternionY = Math::CalcQuaternion(destinationQuaternion, posQuaternion);


	destinationAngleY_ = posQuaternionY.y;
}