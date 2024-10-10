#include "DebugPlayer.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

void DebugPlayer::Initialize() {

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.scale = { 0.5f,0.5f,0.5f };
	worldTransformBase_.translate.x = 2.0f;
	worldTransformBase_.translate.y = -1.5f;
	worldTransformSphere_.Initialize();
	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/white.png"));
	velocity_.x = 0.3f;

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

	if (worldTransformBase_.translate.x <= -10.0f|| worldTransformBase_.translate.x >= 10.0f) {
		velocity_.x = 0;
	}

	worldTransformBase_.UpdateMatrix();
	worldTransformSphere_.UpdateMatrix();

	ImGui::Begin("Setting");
	ImGui::Text("posX%f", GetWorldPosition().x);
	ImGui::Text("posY%f", GetWorldPosition().y);
	ImGui::Text("posZ%f", GetWorldPosition().z);
	ImGui::DragFloat("PosY", &worldTransformBase_.translate.y, 0.1f);
	ImGui::End();
}

void DebugPlayer::Draw(const ViewProjection& camera) {
	//animation_->Draw(worldTransformBase_, camera, false);
	sphere_->Draw(worldTransformBase_, camera, false);
	//RenderCollisionBounds(worldTransformSphere_, camera);
}

// 移動
void DebugPlayer::MoveInitialize() {
	worldTransformBase_.translate.y = -1.5f;
	velocity_.y = 0.0f;
};
void DebugPlayer::MoveUpdata() {
	
	//const float value = 0.7f;
	bool isMove_ = false;
	/*----------移動処理----------*/
	float kCharacterSpeed = 0.3f;
	// 移動量
	velocity_ = { 0.0f, 0.0f, 0.0f };

	
	// 左右移動
	if (Input::PressKey(DIK_A)) {
		if (worldTransformBase_.translate.x >= -10.0f) {
			velocity_.x = -1;
		}
		

	}
	else if (Input::PressKey(DIK_D)) {
		if (worldTransformBase_.translate.x <= 10.0f) {
			velocity_.x = 1;
		}
		
	}

	if (velocity_.x != 0) {
		isMove_ = true;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
	}

	if (isMove_) {
		// 平行移動
		worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity_);
	}

	// ジャンプ
	if (Input::PushKey(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kJump;
	}
};

// ジャンプ
void DebugPlayer::JumpInitialize() {

	// ジャンプ初速
	const float kJumpFirstSpeed = 0.3f;
	velocity_.y = kJumpFirstSpeed;
	
};
void DebugPlayer::JumpUpdata() {
	
	// 移動
	worldTransformBase_.translate += velocity_;
	// 重力加速度
	const float kGravity = 0.03f;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;
	
	if (worldTransformBase_.translate.y <= -1.5f) {
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
