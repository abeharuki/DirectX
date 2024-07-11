#include "DebugPlayer.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

void DebugPlayer::Initialize() {

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.scale = { 3.0f,3.0f,3.0f };
	worldTransformBase_.translate.x = 2.0f;
	worldTransformBase_.translate.y = 1.0f;
	worldTransformSphere_.Initialize();
	worldTransformSphere_.translate.y = 1.0f;
	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/monsterBall.png"));


	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("resources/human", "uvChecker.png", "Human.gltf"));
	animation_->SetAnimationTimer(0.0f, 30.0f);
	AABB aabbSize{ .min{-1.0f,-1.0f,-1.0f},.max{1.0f,1.0f,1.0f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
}

void DebugPlayer::Update() {
	/*
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

	}*/

	

	velocity_ = { 0.0f,0.0f,0.0f };
	// 上下移動
	if (Input::PressKey(DIK_W)) {
		velocity_.z = 0.3f;

	}
	else if (Input::PressKey(DIK_S)) {
		velocity_.z = -0.3f;
	}


	// 左右移動
	if (Input::PressKey(DIK_A)) {
		velocity_.x = -0.3f;

	}
	else if (Input::PressKey(DIK_D)) {
		velocity_.x = 0.3f;
	}


	Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(viewProjection_->rotation_.y);
	velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
	// 現在の位置から移動する位置へのベクトル
	Vector3 sub = (worldTransformSphere_.translate + velocity_) - GetLocalPosition();
	// 平行移動
	worldTransformSphere_.translate = Math::Add(worldTransformSphere_.translate, velocity_);
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
	

	// 回転
	//worldTransformSphere_.rotate.y = Math::LerpShortAngle(worldTransformSphere_.rotate.y, destinationAngleY_, 0.2f);

	worldTransformBase_.UpdateMatrix();
	worldTransformSphere_.UpdateMatrix();

	ImGui::Begin("Setting");
	ImGui::SliderFloat3("pos", &worldTransformBase_.translate.x, -10.0f, 10.0f);
	ImGui::End();
}

void DebugPlayer::Draw(const ViewProjection& camera) {
	//animation_->Draw(worldTransformBase_, camera, false);
	//RenderCollisionBounds(worldTransformSphere_, camera);
	sphere_->Draw(worldTransformSphere_, camera, false);
	RenderCollisionBounds(worldTransformSphere_, camera);
}

// 移動
void DebugPlayer::MoveInitialize() {
	worldTransformBase_.translate.y = 1.0f;
	workAttack_.isAttack = false;
	dash_ = false;
	combo_ = false;
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		const float value = 0.7f;
		// 移動量
		velocity_ = {
		   (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		   (float)joyState.Gamepad.sThumbLY / SHRT_MAX };

		//
		if (Math::Length(velocity_) > value) {
			//animation_->SetAnimatioNumber(4, 2);
			animation_->SetAnimationTimer(0.8f, 30.0f);
		}
		else {
			animation_->SetAnimationTimer(0.0f, 30.0f);
		}
		
	}
	
	
	//
};
void DebugPlayer::MoveUpdata() {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	//noAttack_ = false;
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		const float value = 0.7f;
		bool isMove = false;

		// 移動速度
		const float kCharacterSpeed = 0.5f;
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
			animation_->Update(4);
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
		else {
			animation_->Update(1);
		}

		// ジャンプ
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			behaviorRequest_ = Behavior::kJump;
		}

		// 攻撃
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B) && !preNoAttack_) {
			//behaviorRequest_ = Behavior::kAttack;
		}


		// ダッシュボタンを押したら
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
			//behaviorRequest_ = Behavior::kDash;
		} 


	}
	else {
		animation_->Update(1);
	}


	
};

// ジャンプ
void DebugPlayer::JumpInitialize() {
	worldTransformBase_.translate.y = 1.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	velocity_.y = kJumpFirstSpeed;
	animation_->SetAnimationTimer(0.0f, 60.0f);
	animation_->SetBlend(1, 0.1f);
};
void DebugPlayer::JumpUpdata() {
	animation_->Update(2);
	// 移動
	worldTransformBase_.translate += velocity_;
	// 重力加速度
	const float kGravity = 0.03f;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;
	// 移動速度
	const float kCharacterSpeed = 0.2f;


	if (worldTransformBase_.translate.y <= 1.0f) {
		// ジャンプ終了
		behaviorRequest_ = Behavior::kRoot;
	}
};

// ダッシユ
void DebugPlayer::DashInitialize() {
	workDash_.dashParameter_ = 0;
	worldTransformBase_.rotate.y = destinationAngleY_;
	dash_ = true;
}
void DebugPlayer::DashUpdata() {
	// dashTimer -= 4;

	Vector3 velocity = { 0, 0, workDash_.dashSpeed };

	velocity = Math::TransformNormal(velocity, worldTransformBase_.matWorld_);
	worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity);

	// 既定の時間経過で通常行動に戻る
	if (++workDash_.dashParameter_ >= workDash_.behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}
}


// 攻撃
void DebugPlayer::AttackInitialize(){}
void DebugPlayer::AttackUpdata(){
	animation_->Update(3);
}

void DebugPlayer::OnCollision(Collider* collider){

}

const Vector3 DebugPlayer::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

Vector3 DebugPlayer::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformSphere_.translate.x;
	worldPos.y = worldTransformSphere_.translate.y;
	worldPos.z = worldTransformSphere_.translate.z;
	return worldPos;
}

DebugPlayer::~DebugPlayer(){}
