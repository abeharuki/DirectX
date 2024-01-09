#include "Enemy.h"

void Enemy::Initialize() {

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.rotate.x = 1.65f;
	worldTransformBase_.translate.z = 10.0f;
	
}

void Enemy::Update() { 
	

	if (KeyInput::GetKey(DIK_RIGHTARROW)) {
		velocity_.x = 0.1f;
	} else if (KeyInput::GetKey(DIK_LEFTARROW)) {
		velocity_.x = -0.1f;
		
	}

	if (behaviorRequest_) {
		// 振る舞い変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			MoveInitialize();
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
	
	case Behavior::kDead:
		DeadUpdata();
		break;
	}


	worldTransformBase_.translate += velocity_;
	
	worldTransformBase_.UpdateMatrix();


	ImGui::Begin("Enemy");
	ImGui::SliderFloat3("roto", &worldTransformBase_.rotate.x, -1.0f, 5.0f);
	ImGui::End();

}




void Enemy::DeadInitilize() {

}
void Enemy::DeadUpdata(){};




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