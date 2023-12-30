#include "Renju.h"
#include <numbers>


Renju::~Renju(){};

/// <summary>
/// 初期化
/// </summary>
void Renju::Initialize(){
	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate.x = -2.0f;
	worldTransformBase_.rotate.y = 3.14f;
	bulletModel_.reset(Model::CreateModelFromObj("resources/Renju/cube.obj", "resources/Renju/Bullet.png"));
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Renju::Update(){
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
	case Behavior::kAttack:
		AttackUpdata();
		break;
	case Behavior::kDead:
		break;
	}

	// 回転
	worldTransformBase_.rotate.y =
	    Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_ + 3.14f, 0.2f);

	worldTransformBase_.UpdateMatrix();
};

void Renju::Draw(const ViewProjection& view) {

	// 弾の描画
	for (RenjuBullet* bullet : bullets_) {
		bullet->Draw(view);
	}
	

}


// 移動
void Renju::MoveInitialize(){
	// デスフラグが立った弾を削除
	bullets_.remove_if([](RenjuBullet* bullet) { 
		delete bullet;
		return true;
	});
};
void Renju::MoveUpdata(){
	// プレイヤーに集合
	if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X) && !followPlayer_) {
		followPlayer_ = true;
	}

	// 敵を探す
	if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_Y) ) {
		searchTarget_ = true;
	}
};

// ジャンプ
void Renju::JumpInitialize(){};
void Renju::JumpUpdata(){};


// 攻撃
void Renju::AttackInitialize() { 
	searchTarget_ = false;
	fireTimer_ = 20;
};
void Renju::AttackUpdata(){
	--fireTimer_;

	if (fireTimer_ == 0) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		
		Vector3 vector = Vector3{enemyPos_.x,enemyPos_.y+7,enemyPos_.z} - worldTransformBase_.GetWorldPos();
		vector = Math::Normalize(vector);
		Vector3 velocity = kBulletSpeed * vector;

		// 弾を生成、初期化
		RenjuBullet* newBullet = new RenjuBullet();
		newBullet->Initialize(
		    bulletModel_.get(), worldTransformBase_.translate, {0.5f, 0.5f, 0.5f},
		    worldTransformBase_.rotate, velocity);
		

		bullets_.push_back(newBullet);

		fireTimer_ = 20;
	}

	// デスフラグが立った弾を削除
	bullets_.remove_if([](RenjuBullet* bullet) {
		if (bullet->IsDead()) {

			delete bullet;
			return true;
		}
		return false;
	});

	for (RenjuBullet* bullet : bullets_) {

		bullet->Update();
	}




	// プレイヤーに集合
	if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_X)) {
		behaviorRequest_ = Behavior::kRoot;
		followPlayer_ = true;
		
	}
};


void Renju::followPlayer(Vector3 playerPos) {
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

void Renju::searchTarget(Vector3 enemyPos) {
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
		if (minDistance_*2 <= length) {
			worldTransformBase_.translate =
			    Math::Lerp(worldTransformBase_.translate, enemyPos, 0.02f);
			worldTransformBase_.translate.y = 0.0f;
		} else {
			behaviorRequest_ = Behavior::kAttack;
		}
	}
}

Vector3 Renju::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

Vector3 Renju::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.translate.x;
	worldPos.y = worldTransformBase_.translate.y;
	worldPos.z = worldTransformBase_.translate.z;
	return worldPos;
}