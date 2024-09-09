#include "DebugEnemy.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

void DebugEnemy::Initialize() {

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.scale = { 1.0f,1.0f,1.0f };
	worldTransformBase_.translate.x = 2.0f;
	worldTransformBase_.translate.y = 5.0f;

	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/monsterBall.png"));
	velocity_.x = 0.3f;
	AABB aabbSize{ .min{-0.5f,-0.5f,-0.5f},.max{0.5f,0.5f,0.5f} };
	OBB obb = Math::ConvertAABBToOBB(aabbSize);
	obb.center = { 0.0f,0.0f,0.0f };
	SetOBB(obb);
	SetCollisionPrimitive(kCollisionPrimitiveOBB);
	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(kCollisionMaskEnemy);
}

void DebugEnemy::Update() {


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
	

	ImGui::Begin("Setting");
	ImGui::Text("posX%f", GetWorldPosition().x);
	ImGui::Text("posY%f", GetWorldPosition().y);
	ImGui::Text("posZ%f", GetWorldPosition().z);
	ImGui::DragFloat3("Pos", &worldTransformBase_.translate.x, 0.1f);
	ImGui::DragFloat3("velocity" ,&velocity_.x, 0.1f);
	ImGui::End();
}

void DebugEnemy::Draw(const ViewProjection& camera) {
	sphere_->Draw(worldTransformBase_, camera, false);
	RenderCollisionBounds(worldTransformBase_, camera);
}

// 移動
void DebugEnemy::MoveInitialize() {
	behaviorJumpTime = 10;
	behaviorAttackTime = 30;

	//velocity_.y = 0.0f;
	worldTransformBase_.translate.y = 5.0f;
	velocity_ = { 0.0f,0.0f,0.0f };
	jump_ = false;
};
void DebugEnemy::MoveUpdata() {
	float sub = debugPlayer_->GetWorldPosition().x - worldTransformBase_.translate.x;
	if (sub < 0) {
		sub *= -1;
	}

	//playerの位置が敵から±2の位置にいるかどうか
	if (!Math::isWithinRange(debugPlayer_->GetWorldPosition().x, worldTransformBase_.translate.x, 1.0f)) {
		

		//playerと敵の距離が5離れたいたらジャンプして移動
		if (--behaviorJumpTime <= 0) {
			if (debugPlayer_->GetWorldPosition().x < worldTransformBase_.translate.x) {
				velocity_.x = -0.3f;
			}
			else {
				velocity_.x = 0.3f;
			}
			behaviorRequest_ = Behavior::kJump;
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

	
};

// ジャンプ
void DebugEnemy::JumpInitialize() {
	worldTransformBase_.translate.y = 5.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.5f;
	velocity_.y = kJumpFirstSpeed;
	jump_ = true;
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
	// 移動速度
	const float kCharacterSpeed = 0.2f;

	//ジャンプ中にプレイヤーが真下に来たら攻撃
	if (Math::isWithinRange(debugPlayer_->GetWorldPosition().x, worldTransformBase_.translate.x, 1.0f)) {
		if (worldTransformBase_.translate.y >= 8.5f) {
			//プレイヤーの上にいる時,速度はゼロ
			velocity_.x = 0.0f;
			//クールタイムが終わったら攻撃
			behaviorRequest_ = Behavior::kAttack;
		}

		
	}
	

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
	if (!jump_) {
		const float kJumpFirstSpeed = 0.5f;
		velocity_.y = kJumpFirstSpeed;
	}
	else {
		
	}
	
	
	behaviorMoveTime = 20;

}
void DebugEnemy::AttackUpdata() {
	// 移動
	worldTransformBase_.translate += velocity_;

	if (!jump_) {
		//最高地点まで行ったら勢いよく落ちる
		if (velocity_.y <= 0.0f) {
			attack_ = true;
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
	}
	else {
		attack_ = true;
		velocity_.y += -0.1f;
	}


	if (worldTransformBase_.translate.y <= 5.0f) {
		velocity_.y = 0.0f;
		worldTransformBase_.translate.y = 5.0f;
		
		// 攻撃終了
		if (--behaviorMoveTime <= 0) {
			behaviorRequest_ = Behavior::kRoot;
			attack_ = false;
		}
		
	}
}

void DebugEnemy::OnCollision(Collider* collider) {
	if (collider->GetCollisionAttribute() == kCollisionAttributeLoderWall) {
		ImGui::Begin("DebugEnemy");
		ImGui::Text("hit");
		ImGui::End();
	}
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
	worldPos.x = worldTransformBase_.translate.x;
	worldPos.y = worldTransformBase_.translate.y;
	worldPos.z = worldTransformBase_.translate.z;
	return worldPos;
}

DebugEnemy::~DebugEnemy() {}
