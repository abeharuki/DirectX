#include "Tank.h"


Tank::~Tank() {
	delete behaviorTree_;
	
}

void Tank::Initialize(Animations* animation, std::string skillName) {
	BaseCharacter::Initialize(animation, skillName);
	shield_.reset(Model::CreateModelFromObj("resources/Tank/shield.obj", "resources/white.png"));

	worldTransformShield_.Initialize();
	worldTransformShield_.translate = { 0.0f,1.f,0.45f };
	worldTransformShield_.rotate.y = 3.1415f;


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();

	emitter_ = {
	.translate{0,0,0},
	.count{50},
	.frequency{0.02f},
	.frequencyTime{0.0f},
	.scaleRange{.min{0.1f,0.1f,0.1f},.max{0.1f,0.1f,0.1f}},
	.translateRange{.min{-0.5f,0.0f,-0.5f},.max{0.5f,1.0f,0.5f}},
	.colorRange{.min{1.f,0,0.0f},.max{1.f,0.5f,0.0f}},
	.lifeTimeRange{.min{0.5f},.max{0.5f}},
	.velocityRange{.min{0.f,0.0f,0.f},.max{0.f,0.01f,0.0f}},
	};
	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));
	particle_->SetEmitter(emitter_);

	AABB aabbSize{ .min{-0.5f,-0.0f,-0.4f},.max{0.5f,1.5f,0.4f} };
	SetAABB(aabbSize);
	
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributeTank);
	SetCollisionMask(kCollisionMaskTank);

	// ビヘイビアツリーの初期化
	behaviorTree_ = new BehaviorTree<Tank>(this);
	behaviorTree_->Initialize();
	
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Tank::Update() {
	// 状態が変わった場合にノードの初期化を行う
	if (state_ != previousState_) {
		// 状態に応じた初期化処理を呼び出す
		if (behaviorTree_) {
			behaviorTree_->NodeInitialize();
		}
		previousState_ = state_;  // 現在の状態を記録しておく
	}

	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;

	preHitPlayer_ = isHitPlayer_;
	isHitPlayer_ = false;
	
	editor_.load("Tank");
#ifdef _DEBUG

	editor_.show("TankNode");
	editor_.save("Tank");

#endif // DEBUG

	Relationship();
	BaseCharacter::Update();
	worldTransformShield_.TransferMatrix();
	if (behaviorTree_) {
		behaviorTree_->Update();
	}


	particle_->SetTranslate(worldTransformBase_.translate);

	ImGui::Begin("Tank");
	ImGui::SliderFloat3("pos", &worldTransformBase_.translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("shieldpos", &worldTransformShield_.translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("shieldrotate", &worldTransformShield_.rotate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("enemypos", &enemy_->GetWorldTransformArea().translate.x, -10.0f, 10.0f);
	ImGui::DragFloat3("rotate", &worldTransformBase_.rotate.x);
	ImGui::Text("%d", fireTimer_);
	ImGui::Text("%f", hp_);
	ImGui::End();

	ImGui::Begin("Sprite");
	ImGui::DragFloat("TankHp", &hp_, 1.0f);
	ImGui::End();
};

void Tank::Draw(const ViewProjection& camera) {
	BaseCharacter::Draw(camera);
	
	if (state_ == CharacterState::Unique) {
		shield_->Draw(worldTransformShield_, camera, true);
	}
	particle_->Draw(camera);
	
	RenderCollisionBounds(worldTransformBody_, camera);
}

void Tank::NoDepthDraw(const ViewProjection& camera){
	BaseCharacter::NoDepthDraw(camera);
}

// 移動
void Tank::MoveInitialize() { 
	BaseCharacter::MoveInitialize();
	stanAttack_ = false;
};
void Tank::MoveUpdate() {
	
	BaseCharacter::MoveUpdate();

	if (enemy_->GetBehavior() != Behavior::kStan && !operation_) {
		if (enemy_->IsBehaberAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kNomal && mp_ >= 20) {
			//state_ = NextState("Move", Output3);
		}
	}
	

	if (Input::PressKey(DIK_T)) {
		//state_ = NextState("Move", Output3);
	}

};

// ジャンプ
void Tank::JumpInitialize() {
	BaseCharacter::JumpInitialize();
};
void Tank::JumpUpdate() {
	BaseCharacter::JumpUpdate();
};

// 攻撃
void Tank::AttackInitialize() { 
	BaseCharacter::AttackInitialize();
	fireTimer_ = 40;
};
void Tank::AttackUpdate() {
	--fireTimer_;

	// 追従対象からロックオン対象へのベクトル
	Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

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

	// 敵の座標までの距離
	float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));

	// 距離条件チェック
	if (minDistance_ * 2 <= length && !followPlayer_) {
		state_ = NextState("Attack", Output1);
		searchTarget_ = true;
	}


	isAttack_ = false;
	
	if (fireTimer_ > 10) {
		velocity_ = { 0.0f,0.0f,-0.01f };
		const float kCharacterSpeed = 0.1f;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		worldTransformBase_.translate += velocity_;
	}
	else if (fireTimer_ <= 5 && fireTimer_ > 0) {
		isAttack_ = true;

		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemy_->GetWorldPosition(), 0.2f);
	}
	else if (fireTimer_ <= 0) {
		fireTimer_ = 40;
		coolTime_ = 60;
		state_ = NextState("Attack", Output1);
	}

	BaseCharacter::AttackUpdate();
}


void Tank::UniqueInitialize(){
	mp_ -= 20;
	fireTimer_ = 40;
	stanAttack_ = false;
}
void Tank::UniqueUpdate(){
	--fireTimer_;
	particle_->Update();
	// 追従対象からロックオン対象へのベクトル
	Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

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

	// 敵の座標までの距離
	float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));
	if (length <= 4) {
		stanAttack_ = true;
	}
	if (fireTimer_ > 10) {
		velocity_ = { 0.0f,0.0f,-0.01f };
		const float kCharacterSpeed = 0.1f;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		worldTransformBase_.translate += velocity_;
	}
	else if (fireTimer_ <= 5 && fireTimer_ > 0) {
		
		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemy_->GetWorldPosition(), 0.2f);
	}
	else if (fireTimer_ <= 0) {
		fireTimer_ = 40;
		coolTime_ = 60;
		stanAttack_ = false;
		//state_ = CharacterState::Moveing;
	}

}


void Tank::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	BaseCharacter::DeadInitialize();
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
		hp_ = 21;
		//state_ = CharacterState::Moveing;
		isDead_ = false;
	}

	ImGui::Begin("revival");
	ImGui::Text("T%d", revivalCount_);
	ImGui::Text("%d", isHitPlayer_);
	ImGui::Text("%d", preHitPlayer_);
	ImGui::End();
}

void Tank::Relationship() {
	BaseCharacter::Relationship();

	worldTransformShield_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformShield_.scale, worldTransformShield_.rotate, worldTransformShield_.translate),
		worldTransformBase_.matWorld_);
}

// 衝突を検出したら呼び出されるコールバック関数
void Tank::OnCollision(Collider* collider) {

	BaseCharacter::OnCollision(collider);

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		if (enemy_->isAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
			
			if (stanAttack_) {
				stanAttack_ = false;
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


