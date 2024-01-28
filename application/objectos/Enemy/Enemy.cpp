#include "Enemy.h"
#include <numbers>

void Enemy::Initialize() {

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.rotate.x = 1.65f;
	worldTransformBase_.translate.z = 10.0f;
	clear_ = false;
	time_ = 60*6;
	worldTransformBase_.UpdateMatrix();
}

void Enemy::Update() { 
	

	if (behaviorRequest_) {
		// 振る舞い変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			MoveInitialize();
			break;
		case Behavior::kAttack:
			AttackInitialize();
			break;
		case Behavior::kDead:
			DeadInitilize();
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
	case Behavior::kAttack:
		AttackUpdata();
		break;
	case Behavior::kDead:
		DeadUpdata();
		break;
	}


	
	
	worldTransformBase_.UpdateMatrix();

}



 //移動
void Enemy::MoveInitialize() { 
	time_ = 60*6;
	isAttack_ = false;
};
void Enemy::MoveUpdata() {

	if (time_ < 120) {
		worldTransformBase_.rotate.y += 0.5f;
	}

	if (--time_ <= 0) {
		behaviorRequest_ = Behavior::kAttack;
	}

};

void Enemy::AttackInitialize() { 
	num_ = rand() % 4 + 1; 
	time_ = 30;

}
void Enemy::AttackUpdata() { 
	// 追従対象からロックオン対象へのベクトル
	isAttack_ = true;
	// 回転
	worldTransformBase_.rotate.y =
	    Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_ + 3.14f, 0.2f);
	if (num_ == 1) {
		sub = playerPos_ - GetWorldPosition();
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
		float length = Math::Length(Math::Subract(tankPos_, worldTransformBase_.translate));

		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, playerPos_, 0.2f);

		
	}
	if (num_ == 2) {
		sub = healerPos_ - GetWorldPosition();
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
		float length = Math::Length(Math::Subract(tankPos_, worldTransformBase_.translate));

		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, healerPos_, 0.2f);
		
	}
	if (num_ == 3) {
		sub = renjuPos_ - GetWorldPosition();
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
		float length = Math::Length(Math::Subract(tankPos_, worldTransformBase_.translate));

		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, renjuPos_, 0.2f);
		
	}
	if (num_ == 4) {
		sub = tankPos_ - GetWorldPosition();
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
		float length = Math::Length(Math::Subract(tankPos_, worldTransformBase_.translate));

		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, tankPos_, 0.2f);
		
			
	}

	
	if (--time_ <= 0) {
			behaviorRequest_ = Behavior::kRoot;
	
	}

	
	
	worldTransformBase_.translate.y = 0.0f;
}

void Enemy::DeadInitilize() {

}
void Enemy::DeadUpdata(){
	worldTransformBase_.rotate.z -= 0.01f;
	if (worldTransformBase_.rotate.z < -1.0f) {
		   clear_ = true;
	}
		    
};




Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

Enemy::~Enemy() {}