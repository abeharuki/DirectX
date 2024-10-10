#include "DebugEnemy.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

void DebugEnemy::Initialize() {

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.scale = { 1.0f,1.0f,1.0f };
	worldTransformBase_.translate.x = 2.0f;
	worldTransformBase_.translate.y = 5.0f;
	worldTransformSphere_.Initialize();
	worldTransformSphere_.translate.y = 1.0f;
	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/mahoujin.png"));
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

void DebugEnemy::Update() {

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

	if (worldTransformBase_.translate.x <= -10) {
		velocity_.x = 0.3f;
	}

	if (worldTransformBase_.translate.x >= 10) {
		velocity_.x = -0.3f;
	}

	worldTransformBase_.UpdateMatrix();
	worldTransformSphere_.UpdateMatrix();

	ImGui::Begin("Setting");
	ImGui::Text("posX%f", GetWorldPosition().x);
	ImGui::Text("posY%f", GetWorldPosition().y);
	ImGui::Text("posZ%f", GetWorldPosition().z);
	ImGui::End();
}

void DebugEnemy::Draw(const ViewProjection& camera) {
	//animation_->Draw(worldTransformBase_, camera, false);
	sphere_->Draw(worldTransformBase_, camera, false);
	//RenderCollisionBounds(worldTransformSphere_, camera);
}

// 移動
void DebugEnemy::MoveInitialize() {
	behaviorJumpTime = 30;
	behaviorAttackTime = 60;

	velocity_.y = 0.0f;
	worldTransformBase_.translate.y = 5.0f;
};
void DebugEnemy::MoveUpdata() {
	float sub = debugPlayer_->GetWorldPosition().x - worldTransformBase_.translate.x;
	if (sub < 0) {
		sub *= -1;
	}

	//playerの位置が敵から±2の位置にいるかどうか
	if (!Math::isWithinRange(debugPlayer_->GetWorldPosition().x, worldTransformBase_.translate.x, 2)) {
		if (debugPlayer_->GetWorldPosition().x < worldTransformBase_.translate.x) {
			velocity_.x = -0.3f;
		}
		else {
			velocity_.x = 0.3f;
		}
	}
	else {
		//プレイヤーの上にいる時,速度はゼロ
		velocity_.x = 0.0f;
		//クールタイムが終わったら攻撃
		if (--behaviorAttackTime <= 0) {
			behaviorRequest_ = Behavior::kAttack;
		}
		
	}

	worldTransformBase_.translate += velocity_;

	//playerと敵の距離が5離れたいたらジャンプして移動
	if (sub >= 5) {
		behaviorRequest_ = Behavior::kJump;
	}
};

// ジャンプ
void DebugEnemy::JumpInitialize() {
	worldTransformBase_.translate.y = 5.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.5f;
	velocity_.y = kJumpFirstSpeed;

};
void DebugEnemy::JumpUpdata() {

	// 移動
	worldTransformBase_.translate += velocity_;
	// 重力加速度
	const float kGravity = 0.03f;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;


	if (worldTransformBase_.translate.y <= 5.0f) {
		velocity_.y = 0.0f;
		worldTransformBase_.translate.y = 5.0f;

		// ジャンプ終了
		behaviorRequest_ = Behavior::kRoot;
	}
};

// ダッシユ
void DebugEnemy::DashInitialize() {
	workDash_.dashParameter_ = 0;
	worldTransformBase_.rotate.y = destinationAngleY_;
	dash_ = true;
}
void DebugEnemy::DashUpdata() {
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
void DebugEnemy::AttackInitialize() {
	worldTransformBase_.translate.y = 5.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.5f;
	velocity_.y = kJumpFirstSpeed;
	behaviorMoveTime = 20;
}
void DebugEnemy::AttackUpdata() {
	// 移動
	worldTransformBase_.translate += velocity_;
	//最高地点まで行ったら勢いよく落ちる
	if (velocity_.y <= 0.0f) {
		velocity_.y = -1.0f;
	}
	else {
		// 重力加速度
		const float kGravity = 0.03f;
		// 加速ベクトル
		Vector3 accelerationVector = { 0, -kGravity, 0 };
		// 加速
		velocity_ += accelerationVector;
	}

	if (worldTransformBase_.translate.y <= 5.0f) {
		velocity_.y = 0.0f;
		worldTransformBase_.translate.y = 5.0f;
		// 攻撃終了
		if (--behaviorMoveTime <= 0) {
			behaviorRequest_ = Behavior::kRoot;
		}
		
	}
}

void DebugEnemy::OnCollision(Collider* collider) {

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {}

	if (collider->GetCollisionAttribute() == kCollisionAttributeLoderWall) {}

}

const Vector3 DebugEnemy::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

Vector3 DebugEnemy::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformSphere_.translate.x;
	worldPos.y = worldTransformSphere_.translate.y;
	worldPos.z = worldTransformSphere_.translate.z;
	return worldPos;
}

DebugEnemy::~DebugEnemy() {}
