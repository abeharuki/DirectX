#include "DebugEnemy.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

void DebugEnemy::Initialize() {

	
	transformBase_.scale = { 280.0f,212.0f,1.0f };
	transformBase_.translate.x = 30.0f;
	transformBase_.translate.y = 123.0f;
	velocity_.x = 10.0f;
	enemySprite_.reset(Sprite::Create("resources/Enemy/boss_kari.png"));
	
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

	if (transformBase_.translate.x <= -10) {
		velocity_.x = 10.0f;
	}

	if (transformBase_.translate.x >= 750) {
		velocity_.x = -10.0f;
	}

	enemySprite_->SetTransform(transformBase_);
	

	ImGui::Begin("Setting");
	ImGui::Text("posX%f", transformBase_.translate.x);
	ImGui::Text("posY%f", transformBase_.translate.y);
	ImGui::Text("posZ%f", transformBase_.translate.z);
	ImGui::DragFloat3("Pos", &transformBase_.translate.x, 0.1f);
	ImGui::DragFloat3("Scale", &transformBase_.scale.x, 0.1f);
	ImGui::DragFloat3("velocity" ,&velocity_.x, 0.1f);
	ImGui::End();
}

void DebugEnemy::Draw(const ViewProjection& camera) {
	enemySprite_->Draw();
}

// 移動
void DebugEnemy::MoveInitialize() {
	behaviorJumpTime = 10;
	behaviorAttackTime = 10;

	//velocity_.y = 0.0f;
	transformBase_.translate.y = 5.0f;
	velocity_ = { 0.0f,0.0f,0.0f };
	jump_ = false;
};
void DebugEnemy::MoveUpdata() {
	float sub = debugPlayer_->GetWorldPosition().x - transformBase_.translate.x;
	if (sub < 0) {
		sub *= -1;
	}

	////playerの位置が敵から±2の位置にいるかどうか
	//if (!Math::isWithinRange(debugPlayer_->GetWorldPosition().x, transformBase_.translate.x, 1.0f)) {
	//	

	//	//playerと敵の距離が5離れたいたらジャンプして移動
	//	if (--behaviorJumpTime <= 0) {
	//		if (debugPlayer_->GetWorldPosition().x < transformBase_.translate.x) {
	//			velocity_.x = -0.3f;
	//		}
	//		else {
	//			velocity_.x = 0.3f;
	//		}
	//		behaviorRequest_ = Behavior::kJump;
	//	}
	//}
	//else {
	//	//プレイヤーの上にいる時,速度はゼロ
	//	velocity_.x = 0.0f;
	//	//クールタイムが終わったら攻撃
	//	if (--behaviorAttackTime <= 0) {
	//		behaviorRequest_ = Behavior::kAttack;
	//	}
	//	
	//}

	transformBase_.translate += velocity_;
	if (--behaviorAttackTime <= 0) {
		behaviorRequest_ = Behavior::kAttack;
	}
	
};

// ジャンプ
void DebugEnemy::JumpInitialize() {
	transformBase_.translate.y = 5.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 5.0f;
	velocity_.y = kJumpFirstSpeed;
	jump_ = true;
};
void DebugEnemy::JumpUpdata() {

	// 移動
	transformBase_.translate += velocity_;
	// 重力加速度
	const float kGravity = 0.03f;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;
	// 移動速度
	const float kCharacterSpeed = 0.2f;

	//ジャンプ中にプレイヤーが真下に来たら攻撃
	if (Math::isWithinRange(debugPlayer_->GetWorldPosition().x,transformBase_.translate.x, 1.0f)) {
		if (transformBase_.translate.y >= 8.5f) {
			//プレイヤーの上にいる時,速度はゼロ
			velocity_.x = 0.0f;
			//クールタイムが終わったら攻撃
			behaviorRequest_ = Behavior::kAttack;
		}

		
	}
	

	if (transformBase_.translate.y <= 5.0f) {
		velocity_.y = 0.0f;
		transformBase_.translate.y = 5.0f;

		// ジャンプ終了
		behaviorRequest_ = Behavior::kRoot;
	}
};

// ダッシユ
void DebugEnemy::DashInitialize() {
	
	
}
void DebugEnemy::DashUpdata() {

}

// 攻撃
void DebugEnemy::AttackInitialize() {
	transformBase_.translate.y = 123.0f;
	// ジャンプ初速
	if (!jump_) {
		const float kJumpFirstSpeed = -5.0f;
		velocity_.y = kJumpFirstSpeed;
	}
	else {
		
	}
	
	
	behaviorMoveTime = 20;

}
void DebugEnemy::AttackUpdata() {
	// 移動
	transformBase_.translate += velocity_;
	
	if (!jump_) {
		//最高地点まで行ったら勢いよく落ちる
		if (velocity_.y <= 500.0f) {
			attack_ = true;
			velocity_.y = 1.0f;

		}
		else {
			//重力加速度
			const float kGravity = 3.f;
			// 加速ベクトル
			Vector3 accelerationVector = { 0, kGravity, 0 };
			// 加速
			velocity_ += accelerationVector;
		}
	}
	else {
		attack_ = true;
		velocity_.y += 5.f;
	}


	if (transformBase_.translate.y >= 123.0f) {
		velocity_.y = 0.0f;
		transformBase_.translate.y = 123.0f;
		
		// 攻撃終了
		if (--behaviorMoveTime <= 0) {
			behaviorRequest_ = Behavior::kRoot;
			attack_ = false;
		}
		
	}
}



DebugEnemy::~DebugEnemy() {}
