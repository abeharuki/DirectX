#include "Healer.h"
#include <numbers>

Healer::~Healer(){};

/// <summary>
/// 初期化
/// </summary>
void Healer::Initialize() {
	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate.x = 4.0f;

	worldTransformCane_.Initialize();
	worldTransformCane_.translate = {-0.63f, 0.54f, 0.0f};
	worldTransformCane_.rotate.x = -1.56f;
	worldTransformCane_.scale = {0.5f, 0.5f, 0.5f};
	worldTransformCollision_.Initialize();
	worldTransformCollision_.scale = {0.27f, 0.27f, 1.0f};
	worldTransformCollision_.translate.z = 1.5f;
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Healer::Update() {
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
		JumpUpdata();
		break;
	case Behavior::knock:
		knockUpdata();
		break;
	case Behavior::kAttack:
		AttackUpdata();
		break;
	case Behavior::kDead:
		break;
	}

	Relationship();

	// 回転
	worldTransformBase_.rotate.y =
	    Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_+3.14f, 0.2f);

	worldTransformBase_.UpdateMatrix();
	worldTransformCane_.TransferMatrix();

	
};

// 移動
void Healer::MoveInitialize(){ searchTarget_ = false; };
void Healer::MoveUpdata(){
	// プレイヤーに集合
	if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X) || !searchTarget_) {
		followPlayer_ = true;
	}

	// 敵を探す
	if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_Y)) {
		searchTarget_ = true;
	}
};

// ジャンプ
void Healer::JumpInitialize(){};
void Healer::JumpUpdata(){};

	// ノックバック
void Healer::knockInitialize() { nockTime_ = 30; };
void Healer::knockUpdata() {

	worldTransformBase_.translate += velocity_;
	worldTransformBase_.translate.y = 0;
	if (--nockTime_ <= 0) {
		behaviorRequest_ = Behavior::kRoot;
	}
};


//アタック
void Healer::AttackInitialize() { 
	worldTransformCane_.rotate = {-1.56f,0.0f,0.0f};
	searchTarget_ = false; };
void Healer::AttackUpdata(){
	// プレイヤーの座標までの距離
	float length = Math::Length(Math::Subract(enemyPos_, worldTransformBase_.translate));

	// 距離条件チェック
	if (minDistance_ * 2 <= length && !followPlayer_) {
		behaviorRequest_ = Behavior::kRoot;
		searchTarget_ = true;
	} 

	workAttack_.isAttack = false;
	++workAttack_.attackParameter_;
	uint32_t anticipationTime = 15;
	uint32_t chargeTime = anticipationTime + 10;
	uint32_t swingTime = anticipationTime + chargeTime + 5;

	if (workAttack_.attackParameter_ < anticipationTime) {
		worldTransformCane_.rotate.x += 0.04f;
	}

	if (workAttack_.attackParameter_ >= anticipationTime &&
	    workAttack_.attackParameter_ < chargeTime) {
		worldTransformCane_.rotate.x -= 0.0f;
	}

	if (workAttack_.attackParameter_ >= chargeTime && workAttack_.attackParameter_ < swingTime) {
		worldTransformCane_.rotate.x -= 0.15f;
		workAttack_.isAttack = true;
	}

	 // 攻撃の合計時間
	uint32_t totalTime = anticipationTime +chargeTime +swingTime;


	if (workAttack_.attackParameter_ >= swingTime && workAttack_.attackParameter_ < totalTime) {
		workAttack_.attackParameter_ = 0;
		searchTarget_ = true;
	}


	// プレイヤーに集合
	if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X)) {
		behaviorRequest_ = Behavior::kRoot;
		workAttack_.isAttack = false;
		followPlayer_ = true;
		searchTarget_ = false;
	}
};

//追従
void Healer::followPlayer(Vector3 playerPos) {
	if (followPlayer_) {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = playerPos - GetWorldPosition();

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

		// プレイヤーの座標までの距離
		float length = Math::Length(Math::Subract(playerPos, worldTransformBase_.translate));

		// 距離条件チェック
		if (minDistance_ <= length) {
			worldTransformBase_.translate =
			    Math::Lerp(worldTransformBase_.translate, playerPos, 0.02f);
			worldTransformBase_.translate.y = 0.0f;
		} else {
			followPlayer_ = false;
		}
	}
	
}
// 敵を探す
void Healer::searchTarget(Vector3 enemyPos) {
	enemyPos_ = enemyPos;
	if (!followPlayer_ && searchTarget_) {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemyPos - GetWorldPosition();

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

		// プレイヤーの座標までの距離
		float length = Math::Length(Math::Subract(enemyPos, worldTransformBase_.translate));

		// 距離条件チェック
		if (minDistance_ - 8 <= length) {
			worldTransformBase_.translate =
			    Math::Lerp(worldTransformBase_.translate, enemyPos, 0.02f);
			worldTransformBase_.translate.y = 0.0f;
		} else {

			behaviorRequest_ = Behavior::kAttack;
		}
	}
}

// 親子関係
void Healer::Relationship() {

	worldTransformCane_.matWorld_ = Math::Multiply(
	    Math::MakeAffineMatrix(
	        worldTransformCane_.scale, worldTransformCane_.rotate,
	        worldTransformCane_.translate),
	    worldTransformBase_.matWorld_);

	worldTransformCollision_.matWorld_ = Math::Multiply(
	    Math::MakeAffineMatrix(
	        worldTransformCollision_.scale, worldTransformCollision_.rotate,
	        worldTransformCollision_.translate),
	    worldTransformCane_.matWorld_);
}

// 衝突を検出したら呼び出されるコールバック関数
void Healer::OnAllyCollision(const WorldTransform& worldTransform) {
	const float kSpeed = 0.4f;
	float sub = worldTransformBase_.matWorld_.m[3][0] - GetWorldPosition().x;
	if (sub < 0) {
		allyVelocity = {kSpeed, 0.0f, kSpeed};
	} else {
		allyVelocity = {-kSpeed, 0.0f, kSpeed};
	}
	
	
	allyVelocity = Math::TransformNormal(allyVelocity, worldTransform.matWorld_);
	worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, allyVelocity);
}
void Healer::OnCollision(const WorldTransform& worldTransform) {
	const float kSpeed = 8.0f;
	velocity_ = {0.0f, 0.0f, kSpeed};
	velocity_ = Math::TransformNormal(velocity_, worldTransform.matWorld_);
	behaviorRequest_ = Behavior::knock;

	ImGui::Begin("Player");
	ImGui::End();
};

Vector3 Healer::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

Vector3 Healer::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.translate.x;
	worldPos.y = worldTransformBase_.translate.y;
	worldPos.z = worldTransformBase_.translate.z;
	return worldPos;
}