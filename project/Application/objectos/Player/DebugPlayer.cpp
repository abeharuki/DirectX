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

	}

	worldTransformBase_.UpdateMatrix();
	worldTransformSphere_.UpdateMatrix();

	ImGui::Begin("Setting");
	ImGui::Text("posX%f", GetWorldPosition().x);
	ImGui::Text("posY%f", GetWorldPosition().y);
	ImGui::Text("posZ%f", GetWorldPosition().z);
	ImGui::End();
}

void DebugPlayer::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformBase_, camera, false);
	/*sphere_->Draw(worldTransformSphere_, camera, false);
	RenderCollisionBounds(worldTransformSphere_, camera);*/
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
	

	behaviorRequest_ = Behavior::kJump;
	
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
	
	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {}

	if (collider->GetCollisionAttribute() == kCollisionAttributeLoderWall) {}
	
}

const Vector3 DebugPlayer::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformSphere_.matWorld_.m[3][0];
	worldPos.y = worldTransformSphere_.matWorld_.m[3][1];
	worldPos.z = worldTransformSphere_.matWorld_.m[3][2];
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
