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
	case Behavior::kDead:
		break;
	}

	// 回転
	worldTransformBase_.rotate.y =
	    Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_+3.14f, 0.2f);

	worldTransformBase_.UpdateMatrix();
};

// 移動
void Healer::MoveInitialize(){};
void Healer::MoveUpdata(){
	// プレイヤーに集合
	if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X) && !followPlayer_) {
		followPlayer_ = true;
	}

};

// ジャンプ
void Healer::JumpInitialize(){};
void Healer::JumpUpdata(){};

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