#include "Tank.h"
#include <numbers>

Tank::~Tank(){};

/// <summary>
/// 初期化
/// </summary>
void Tank::Initialize() {
	// 初期化
	worldTransformBase_.Initialize();
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Tank::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;

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
		break;
	}

	// 回転
	worldTransformBase_.rotate.y =
	    Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_ + 3.14f, 0.2f);

	worldTransformBase_.UpdateMatrix();
};

// 移動
void Tank::MoveInitialize(){ searchTarget_ = false; };
void Tank::MoveUpdata(){

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
void Tank::JumpInitialize(){};
void Tank::JumpUpdata(){};

	// ノックバック
void Tank::knockInitialize() { nockTime_ = 30; };
void Tank::knockUpdata() {

	worldTransformBase_.translate += velocity_;
	worldTransformBase_.translate.y = 0;
	if (--nockTime_ <= 0) {
		behaviorRequest_ = Behavior::kRoot;
	}
};


// 攻撃
void Tank::AttackInitialize(){ searchTarget_ = false; };
void Tank::AttackUpdata(){
	--fireTimer_;

	// 追従対象からロックオン対象へのベクトル
	Vector3 sub = enemyPos_ - GetWorldPosition();

	// y軸周りの回転
	if (sub.z != 0.0) {
		destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

		if (sub.z < 0.0) {
			destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> - destinationAngleY_
			                                    : -std::numbers::pi_v<float> - destinationAngleY_;
		}
	} else {
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
	if (fireTimer_>10) {
		worldTransformBase_.translate = Math::Lerp(
		    worldTransformBase_.translate,
		    {worldTransformBase_.translate.x, worldTransformBase_.translate.y,
		     worldTransformBase_.translate.z - 2.0f},
		    0.05f);
	} else if (fireTimer_ > 5) {
		attack_ = true;
		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemyPos_, 0.2f);
	} else if (fireTimer_ >0 ) {
		worldTransformBase_.translate = Math::Lerp(
		    worldTransformBase_.translate,
		    {worldTransformBase_.translate.x, worldTransformBase_.translate.y,
		     worldTransformBase_.translate.z - 4.0f},
		    0.2f);
		fireTimer_ = 40;
	}

	
	
	
	// プレイヤーに集合
	if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X)) {
		behaviorRequest_ = Behavior::kRoot;
		followPlayer_ = true;
		searchTarget_ = false;
	}
};


	// プレイヤーに追従
void Tank::followPlayer(Vector3 playerPos){
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
		if (minDistance_-8 <= length) {
			worldTransformBase_.translate =
			    Math::Lerp(worldTransformBase_.translate, enemyPos, 0.02f);
			worldTransformBase_.translate.y = 0.0f;
		} else {
			searchTarget_ = false;
			behaviorRequest_ = Behavior::kAttack;
			
		}
	}
}

// 衝突を検出したら呼び出されるコールバック関数
void Tank::OnAllyCollision(const WorldTransform& worldTransform){
	const float kSpeed = 0.4f;
	float sub = worldTransformBase_.matWorld_.m[3][0] - GetWorldPosition().x;
	if (sub < 0) {
		allyVelocity = {kSpeed, 0.0f, kSpeed};
	} else {
		allyVelocity = {-kSpeed, 0.0f, kSpeed};
	}
	
	allyVelocity = Math::TransformNormal(allyVelocity, worldTransform.matWorld_);
	worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, allyVelocity);
};
void Tank::OnCollision(const WorldTransform& worldTransform) {
	const float kSpeed = 8.0f;
	velocity_ = {0.0f, 0.0f, kSpeed};
	velocity_ = Math::TransformNormal(velocity_, worldTransform.matWorld_);
	behaviorRequest_ = Behavior::knock;

	ImGui::Begin("Player");
	ImGui::End();
};

Vector3 Tank::GetWorldPosition() {
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