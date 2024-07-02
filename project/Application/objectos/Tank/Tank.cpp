#include "Tank.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

Tank::~Tank() {};

/// <summary>
/// 初期化
/// </summary>
void Tank::Initialize() {
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/AnimatedCube", "tex.png", "bound3.gltf"));


	// 初期化
	worldTransformBase_.Initialize();
	worldTransformHead_.Initialize();
	//worldTransformHead_.rotate.y = 3.14f;
	
	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].Initialize();
		worldTransformHp_[i].translate.y = 1.5f;
		worldTransformHp_[i].translate.x = (float(i) * 0.5f) - 0.5f;
		worldTransformHp_[i].scale = { 0.5f,0.5f,0.5f };
	}

	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformHead_.TransferMatrix();

	hitCount_ = 0;

	AABB aabbSize{ .min{-0.5f,-0.2f,-0.25f},.max{0.5f,0.2f,0.25f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributeTank);
	SetCollisionMask(kCollisionMaskTank);
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Tank::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;

	
	preHitPlayer_ = isHitPlayer_;
	isHitPlayer_ = false;


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
		case Behavior::knock:
			knockInitialize();
			break;
		case Behavior::kAttack:
			AttackInitialize();
		case Behavior::kDead:
			DeadInitialize();
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
		JumpUpdata();
		break;
	case Behavior::knock:
		knockUpdata();
		break;
	case Behavior::kAttack:
		AttackUpdata();
	case Behavior::kDead:
		DeadUpdate();
		break;
	}

	Relationship();

	// 回転
	worldTransformBase_.rotate.y =
		Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);

	worldTransformBase_.UpdateMatrix();
	worldTransformHead_.TransferMatrix();
	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].TransferMatrix();
	}
	if (nockBack_) {
		animation_->SetLoop(false);
		animation_->Update(0);
	}
};

void Tank::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformHead_, camera,true);

}

// 移動
void Tank::MoveInitialize() { searchTarget_ = false; };
void Tank::MoveUpdata() {

	// プレイヤーに集合
	if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X) || !searchTarget_) {
		followPlayer_ = true;
	}

	// 敵を探す
	if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_Y)) {
		searchTarget_ = true;
	}


	if (hitCount_ <= 0) {
		behaviorRequest_ = Behavior::kDead;
	}
};

// ジャンプ
void Tank::JumpInitialize() {};
void Tank::JumpUpdata() {};

// ノックバック
void Tank::knockInitialize() { 
	nockTime_ = 30;
	animation_->SetAnimationTimer(0, 8.0f);
	animation_->SetpreAnimationTimer(0);
	nockBack_ = true;
};
void Tank::knockUpdata() {

	worldTransformBase_.translate += velocity_;
	worldTransformBase_.translate.y = 0;
	if (--nockTime_ <= 0) {
		nockBack_ = false;
		if (hitCount_ <= 0) {
			behaviorRequest_ = Behavior::kDead;
		}
		else {
			behaviorRequest_ = Behavior::kRoot;
		}
		animation_->SetAnimationTimer(0, 9.0f);
		animation_->SetpreAnimationTimer(0);
	}
};


// 攻撃
void Tank::AttackInitialize() { searchTarget_ = false; };
void Tank::AttackUpdata() {
	--fireTimer_;

	// 追従対象からロックオン対象へのベクトル
	Vector3 sub = enemyPos_ - GetWorldPosition();

	// y軸周りの回転
	if (sub.z != 0.0) {
		destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

		if (sub.z < 0.0) {
			destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> -destinationAngleY_
				: -std::numbers::pi_v<float> -destinationAngleY_;
		}
	}
	else {
		destinationAngleY_ =
			(sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f : -std::numbers::pi_v<float> / 2.0f;
	}

	// プレイヤーの座標までの距離
	float length = Math::Length(Math::Subract(enemyPos_, worldTransformBase_.translate));

	// 距離条件チェック
	if (minDistance_ * 2 <= length && !followPlayer_) {
		behaviorRequest_ = Behavior::kRoot;
		searchTarget_ = true;
	}


	attack_ = false;
	if (fireTimer_ > 10) {
		worldTransformBase_.translate = Math::Lerp(
			worldTransformBase_.translate,
			{ worldTransformBase_.translate.x, worldTransformBase_.translate.y,
			 worldTransformBase_.translate.z - 2.0f },
			0.05f);
	}
	else if (fireTimer_ > 5) {
		attack_ = true;
		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemyPos_, 0.2f);
	}
	else if (fireTimer_ > 0) {
		worldTransformBase_.translate = Math::Lerp(
			worldTransformBase_.translate,
			{ worldTransformBase_.translate.x, worldTransformBase_.translate.y,
			 worldTransformBase_.translate.z - 4.0f },
			0.2f);
		fireTimer_ = 40;
	}




	// プレイヤーに集合
	if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X)) {
		behaviorRequest_ = Behavior::kRoot;
		followPlayer_ = true;
		searchTarget_ = false;
	}
};

void Tank::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	isDead_ = true;
}
void Tank::DeadUpdate() {
	if (isHitPlayer_ != preHitPlayer_) {
		if (Input::GetInstance()->GetPadConnect()) {
			if (Input::GetInstance()->GetPadButton(XINPUT_GAMEPAD_B)) {
				//復活時間
				revivalCount_++;
			}
			else {
				revivalCount_--;
			}
		}
		else {
			if (Input::GetInstance()->PressKey(DIK_B)) {
				//復活時間
				revivalCount_++;
			}
			else {
				revivalCount_--;
			}
		}
		
		
	}
	else {
		revivalCount_--;
	}
	
	if (revivalCount_ <= 0) {
		revivalCount_ = 0;
	}

	if (revivalCount_ >= 60) {
		hitCount_ = 1;
		behaviorRequest_ = Behavior::kRoot;
		isDead_ = false;
	}

	ImGui::Begin("revival");
	ImGui::Text("T%d", revivalCount_);
	ImGui::Text("%d", isHitPlayer_);
	ImGui::Text("%d", preHitPlayer_);
	ImGui::End();
}

// プレイヤーに追従
void Tank::followPlayer(Vector3 playerPos) {
	if (followPlayer_) {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = playerPos - GetWorldPosition();

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

		// プレイヤーの座標までの距離
		float length = Math::Length(Math::Subract(playerPos, worldTransformBase_.translate));

		// 距離条件チェック
		if (minDistance_ <= length) {
			worldTransformBase_.translate =
				Math::Lerp(worldTransformBase_.translate, playerPos, 0.02f);
			worldTransformBase_.translate.y = 0.0f;
		}
		else {
			followPlayer_ = false;
		}
	}
};

// 敵を探す
void Tank::searchTarget(Vector3 enemyPos) {
	enemyPos_ = enemyPos;
	if (!followPlayer_ && searchTarget_) {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemyPos - GetWorldPosition();

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

		// プレイヤーの座標までの距離
		float length = Math::Length(Math::Subract(enemyPos, worldTransformBase_.translate));

		// 距離条件チェック
		if (minDistance_ - 8 <= length) {
			worldTransformBase_.translate =
				Math::Lerp(worldTransformBase_.translate, enemyPos, 0.02f);
			worldTransformBase_.translate.y = 0.0f;
		}
		else {
			searchTarget_ = false;
			behaviorRequest_ = Behavior::kAttack;

		}
	}
}

void Tank::Relationship() {
	worldTransformHead_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformHead_.scale, worldTransformHead_.rotate, worldTransformHead_.translate),
		worldTransformBase_.matWorld_);

	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].matWorld_ = Math::MakeScaleMatrix(worldTransformHp_[i].scale) * billboardMatrix * Math::MakeTranslateMatrix(Vector3(worldTransformBase_.translate.x + worldTransformHp_[i].translate.x, worldTransformBase_.translate.y + worldTransformHp_[i].translate.y, worldTransformBase_.translate.z));

	}
}

// 衝突を検出したら呼び出されるコールバック関数
void Tank::OnAllyCollision(const WorldTransform& worldTransform) {

};
void Tank::OnCollision(const WorldTransform& worldTransform) {
	const float kSpeed = 3.0f;
	velocity_ = { 0.0f, 0.0f, -kSpeed };
	velocity_ = Math::TransformNormal(velocity_, worldTransform.matWorld_);
	if (hitCount_ > 0) {
		behaviorRequest_ = Behavior::knock;
	}
	

	isHit_ = true;
	if (isHit_ != preHit_) {
		--hitCount_;

	}

};
void Tank::OnCollision(Collider* collider) {

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		if (isEnemyAttack_) {
			const float kSpeed = 3.0f;
			velocity_ = { 0.0f, 0.0f, -kSpeed };
			velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
			if (hitCount_ > 0) {
				behaviorRequest_ = Behavior::knock;
			}
			isHit_ = true;

			if (isHit_ != preHit_) {
				--hitCount_;

			}

		}

	}

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		isHitPlayer_ = true;
	}

	if (
		collider->GetCollisionAttribute() == kCollisionAttributeHealer ||
		collider->GetCollisionAttribute() == kCollisionAttributeRenju) {
		const float kSpeed = 0.02f;
		float subX = collider->GetWorldTransform().matWorld_.m[3][0] - GetWorldPosition().x;
		float subZ = collider->GetWorldTransform().matWorld_.m[3][2] - GetWorldPosition().z;
		if (subX <= 0) {
			allyVelocity.x = kSpeed;
		}
		else {
			allyVelocity.x = -kSpeed;
		}

		if (subZ <= 0) {
			allyVelocity.z = kSpeed;
		}
		else {
			allyVelocity.z = -kSpeed;
		}


		worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, allyVelocity);
	}	
}

const Vector3 Tank::GetWorldPosition() const {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}
Vector3 Tank::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.translate.x;
	worldPos.y = worldTransformBase_.translate.y;
	worldPos.z = worldTransformBase_.translate.z;
	return worldPos;
}