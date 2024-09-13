#include "DebugEnemy.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>

void DebugEnemy::Initialize() {
	transformBase_.scale = { 280.0f,212.0f,1.0f };
	transformBase_.translate.x = 30.0f;
	transformBase_.translate.y = 123.0f;
	transformCore_.scale = { 40.0f,20.0f,0.0f };

	transformUI_[0].translate = { 284.0f,29.6f,0.0f };
	transformUI_[0].scale = { 500.0f,30.0f,0.0f };
	transformUI_[1].translate = { 285.0f,34.5f,0.0f };
	transformUI_[1].scale = { 494.0f,21.0f,0.0f };
	transformUI_[2].translate = { 240.0f,9.0f,0.0f };
	transformUI_[2].scale = { 70.0f,70.0f,0.0f };

	coreSub_ = { 47.0f,92.0f };
	velocity_.x = 10.0f;
	enemySprite_.reset(Sprite::Create("resources/Enemy/boss_kari.png"));
	enemySprite_->SetAnchorPoint({ 0.25f,0.0f });

	enemyUI_[0].reset(Sprite::Create("resources/Enemy/boss_HpFlame.png"));
	enemyUI_[1].reset(Sprite::Create("resources/Enemy/boss_HpGauge.png"));
	enemyUI_[2].reset(Sprite::Create("resources/Enemy/boss_HpIcon.png"));


	damageSound_ = AudioHelper{ "resources/audio/boss_crash.wav" };

	hitBody_ = false;
	hitCore_ = false;
}

void DebugEnemy::Update() {
	preHitBody_ = hitBody_;
	hitBody_ = false;
	preHitCore_ = hitCore_;
	hitCore_ = false;

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
	case Behavior::kDead:
		DeadUpdata();
		break;
	}



	if (transformBase_.translate.x >= 930 || transformBase_.translate.x <= -30) {
		velocity_.x = 0.0f;
		//velocity_.x = -10.0f;
	}
	if (transformUI_[1].scale.x <= 0) {
		transformUI_[1].scale.x = 0.0f;
	}
	for (int i = 0; i < 3; ++i) {
		enemyUI_[i]->SetTransform(transformUI_[i]);
	}
	enemySprite_->SetTransform(transformBase_);
	Vector3 playerPos = debugPlayer_->GetWorldPosition();
	transformCore_.translate = (transformBase_.translate + Vector3{ coreSub_.x,coreSub_.y,0.0f });

	Damage();
	--behaviorStopTime;
	ImGui::Begin("Enemy");
	ImGui::Text("posX%f", transformBase_.translate.x);
	ImGui::Text("posY%f", transformBase_.translate.y);
	ImGui::Text("posZ%f", transformBase_.translate.z);
	ImGui::DragFloat3("Pos", &transformBase_.translate.x, 0.1f);
	ImGui::DragFloat3("playerPos", &playerPos.x, 0.1f);
	ImGui::DragFloat3("velocity", &velocity_.x, 0.1f);
	ImGui::DragFloat3("CoreTranslate", &transformCore_.translate.x, 0.1f);
	ImGui::DragFloat2("CoreSub", &coreSub_.x, 0.1f);
	ImGui::DragFloat3("CoreScale", &transformCore_.scale.x, 0.1f);

	ImGui::Text("stopTime%d", behaviorStopTime);
	ImGui::End();
}

void DebugEnemy::Draw(const ViewProjection &camera) {
	enemySprite_->Draw();
	for (int i = 0; i < 3; ++i) {
		enemyUI_[i]->Draw();
	}
}

// 移動
void DebugEnemy::MoveInitialize() {

	//velocity_.y = 0.0f;
	transformBase_.translate.y = 123.0f;
	velocity_ = { 0.0f,0.0f,0.0f };
	jump_ = false;
};
void DebugEnemy::MoveUpdata() {

	if (behaviorStopTime >= 0) {
		float sub = debugPlayer_->GetWorldPosition().x - transformBase_.translate.x + 80;
		if (sub < 0) {
			sub *= -1;
		}

		if (sub >= 600) {
			jumpAttack_ = true;
			if (debugPlayer_->GetWorldPosition().x < transformBase_.translate.x + 80) {
				velocity_.x = -12.0f;
			}
			else {
				velocity_.x = 12.0f;
			}
			behaviorRequest_ = Behavior::kJump;
		}
		else {
			//playerの位置が敵から±2の位置にいるかどうか
			if (!Math::isWithinRange(debugPlayer_->GetWorldPosition().x, transformBase_.translate.x + 80, 80.0f)) {


				//playerと敵の距離が5離れたいたらジャンプして移動
				if (debugPlayer_->GetWorldPosition().x - 60 < transformBase_.translate.x) {
					for (int i = 0; i < 16; ++i) {
						if (Math::IsBoxCollision(transformBase_.translate.x - 52, transformBase_.translate.y - 10, transformBase_.scale.x * 0.8f, transformBase_.scale.y,
							transformBarr_[i].translate.x, transformBarr_[i].translate.y, transformBarr_[i].scale.x, transformBarr_[i].scale.y)) {
							velocity_.x = 0.0f;
							break;
						}
						else {
							velocity_.x = -10.0f;
						}
					}


				}
				else {
					for (int i = 0; i < 16; ++i) {
						if (Math::IsBoxCollision(transformBase_.translate.x - 32, transformBase_.translate.y - 10, transformBase_.scale.x * 0.8f, transformBase_.scale.y,
							transformBarr_[i].translate.x, transformBarr_[i].translate.y, transformBarr_[i].scale.x, transformBarr_[i].scale.y)) {
							velocity_.x = 0.0f;
							break;
						}
						else {
							velocity_.x = 10.0f;
						}
					}

				}

				if (velocity_.x != 0) {
					--behaviorJumpTime;
				}
				if (behaviorJumpTime <= 0) {
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
		}



		for (int i = 0; i < 16; ++i) {
			ResolveBoxCollision(transformBase_.translate.x - 42, transformBase_.translate.y - 10, transformBase_.scale.x * 0.8f, transformBase_.scale.y,
				transformBarr_[i].translate.x, transformBarr_[i].translate.y, transformBarr_[i].scale.x, transformBarr_[i].scale.y);
		}



		transformBase_.translate += velocity_;

		if (transformUI_[1].scale.x <= 0) {
			behaviorRequest_ = Behavior::kDead;
		}
	}
	else {
		if (behaviorStopTime < -60) {
			behaviorStopTime = 300;
		}
	}





};

// ジャンプ
void DebugEnemy::JumpInitialize() {
	transformBase_.translate.y = 123.0f;
	// ジャンプ初速
	if (!jumpAttack_) {
		const float kJumpFirstSpeed = -10.0f;
		velocity_.y = kJumpFirstSpeed;
	}
	else {
		const float kJumpFirstSpeed = -13.0f;
		velocity_.y = kJumpFirstSpeed;
	}

	jump_ = true;
	behaviorJumpTime = 10;
};
void DebugEnemy::JumpUpdata() {



	// 移動
	transformBase_.translate += velocity_;
	// 重力加速度
	const float kGravity = 0.5f;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;


	for (int i = 0; i < 16; ++i) {
		ResolveBoxCollision(transformBase_.translate.x - 42, transformBase_.translate.y - 10, transformBase_.scale.x * 0.8f, transformBase_.scale.y,
			transformBarr_[i].translate.x, transformBarr_[i].translate.y, transformBarr_[i].scale.x, transformBarr_[i].scale.y);
	}
	if (hitBody_) {
		velocity_.x = 0.0f;

	}

	//ジャンプ中にプレイヤーが真下に来たら攻撃
	if (Math::isWithinRange(debugPlayer_->GetWorldPosition().x, transformBase_.translate.x + 80, 80.0f)) {
		if (!jumpAttack_) {
			if (velocity_.y >= 0) {
				//プレイヤーの上にいる時,速度はゼロ
				velocity_.x = 0.0f;
				//クールタイムが終わったら攻撃
				behaviorRequest_ = Behavior::kAttack;
			}
		}
		else {
			//プレイヤーの上にいる時,速度はゼロ
			velocity_.x = 0.0f;
			//クールタイムが終わったら攻撃
			behaviorRequest_ = Behavior::kAttack;
		}


	}

	if (jumpAttack_) {
		if (velocity_.y >= 10) {
			//クールタイムが終わったら攻撃
			behaviorRequest_ = Behavior::kAttack;
		}
	}

	if (transformBase_.translate.y >= 123.0f) {
		velocity_.y = 0.0f;
		transformBase_.translate.y = 123.0f;

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

	velocity_ = { 0.0f,0.0f,0.0f };
	// ジャンプ初速
	if (!jump_) {
		transformBase_.translate.y = 123.0f;
		const float kJumpFirstSpeed = -10.0f;
		velocity_.y = kJumpFirstSpeed;
	}
	else {
		const float kJumpFirstSpeed = -5.0f;
		velocity_.y = kJumpFirstSpeed;
	}


	if (!jumpAttack_) {
		behaviorMoveTime = 40;
	}
	else {
		behaviorMoveTime = 60;
	}
	behaviorAttackTime = 15;


}
void DebugEnemy::AttackUpdata() {
	// 移動
	transformBase_.translate += velocity_;

	if (!jump_) {
		//最高地点まで行ったら勢いよく落ちる
		if (velocity_.y >= 0.0f) {
			attack_ = true;
			velocity_.y = 20.0f;

		}
		else {
			//重力加速度
			const float kGravity = 0.5f;
			// 加速ベクトル
			Vector3 accelerationVector = { 0, kGravity, 0 };
			// 加速
			velocity_ += accelerationVector;
		}
	}
	else {
		//最高地点まで行ったら勢いよく落ちる
		if (velocity_.y >= 0.0f) {
			attack_ = true;
			velocity_.y = 20.0f;

		}
		else {
			//重力加速度
			const float kGravity = 0.5f;
			// 加速ベクトル
			Vector3 accelerationVector = { 0, kGravity, 0 };
			// 加速
			velocity_ += accelerationVector;
		}
	}


	if (Math::isWithinRange(debugPlayer_->GetWorldPosition().x, transformBase_.translate.x + 80, 80.0f)) {
	}

	if (transformBase_.translate.y >= 123.0f) {
		velocity_.y = 0.0f;
		transformBase_.translate.y = 123.0f;

		if (behaviorMoveTime < 40) {
			attack_ = false;
		}

		// 攻撃終了
		if (--behaviorMoveTime <= 0) {



			behaviorRequest_ = Behavior::kRoot;
			jumpAttack_ = false;

		}

	}
}

void DebugEnemy::DeadInitilize()
{
}

void DebugEnemy::DeadUpdata()
{
}

void DebugEnemy::Damage() {

	//コアの当たり判定
	for (int i = 0; i < 16; ++i) {
		if (Math::IsBoxCollision(transformCore_.translate.x, transformCore_.translate.y, transformCore_.scale.x, transformCore_.scale.y,
			transformBarr_[i].translate.x, transformBarr_[i].translate.y, transformBarr_[i].scale.x, transformBarr_[i].scale.y)) {

			hitCore_ = true;

			if (hitCore_ != preHitCore_) {
				damageSound_.Play(false, 0.75f);
				if (transformUI_[1].scale.x > 0) {
					transformUI_[1].scale.x -= 50;
				}

			}
		}

	}


}



DebugEnemy::~DebugEnemy() {}
