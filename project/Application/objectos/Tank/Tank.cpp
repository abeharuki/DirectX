#include "Tank.h"
#include <ParticleManager.h>


Tank::~Tank() {
	delete behaviorTree_;
	
}

void Tank::Initialize(Animations* animation, std::string skillName) {
	BaseCharacter::Initialize(animation, skillName);
	barrierModel_.reset(Model::CreateModelFromObj("resources/Tank/barrier.obj", "resources/Tank/b.png"));
	barrierModel_->SetBlendMode(BlendMode::kNormal);
	barrierModel_->SetColor(TankConstants::kBarrierColor);
	worldTransformBarrier_.Initialize();
	worldTransformBarrier_.scale = TankConstants::kBarrierScale;

	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();

	worldTransformShadow_.translate = { worldTransformBase_.translate.x,AllyAIConstants::kShadowTranslateOffset,worldTransformBase_.translate.z };
	worldTransformShadow_.UpdateMatrix();

	emitter_ = {
	.translate{0,0,0},
	.count{50},
	.frequency{0.02f},
	.frequencyTime{0.0f},
	.scaleRange{.min{0.1f,0.1f,0.1f},.max{0.1f,0.1f,0.1f}},
	.translateRange{.min{-0.5f,0.0f,-0.5f},.max{0.5f,1.0f,0.5f}},
	.colorRange{.min{1.f,0,0.0f},.max{1.f,0.5f,0.0f}},
	.alphaRange{.min{1.0f},.max{1.0f}},
	.lifeTimeRange{.min{0.5f},.max{0.5f}},
	.velocityRange{.min{0.f,0.0f,0.f},.max{0.f,0.01f,0.0f}},
	};
	particle_ = ParticleManager::Create("resources/particle/circle.png", 10);
	particle_->SetEmitter(emitter_);

	barrierThreshold_ = TankConstants::kBarrierThreshold;

	AABB aabbSize{ .min{-0.7f,-0.0f,-0.4f},.max{0.7f,1.5f,0.4f} };
	SetAABB(aabbSize);
	
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributeTank);
	SetCollisionMask(kCollisionMaskTank);

	// ビヘイビアツリーの初期化
	behaviorTree_ = new BehaviorTree<Tank>(this);
	behaviorTree_->Initialize();
	
	distance_ = TankConstants::kTargetDistance;
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Tank::Update() {
	editor_.load("Tank");
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
	
	Relationship();
	BaseCharacter::Update();
	worldTransformBarrier_.UpdateMatrix();
	if (behaviorTree_) {
		behaviorTree_->Update();
	}


	particle_->SetTranslate(worldTransformBase_.translate);
	barrierModel_->SetThreshold(barrierThreshold_);

	ImGui::Begin("Tank");
	ImGui::SliderFloat3("pos", &worldTransformBase_.translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("shieldpos", &worldTransformBarrier_.translate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("shieldrotate", &worldTransformBarrier_.rotate.x, -10.0f, 10.0f);
	ImGui::SliderFloat3("enemypos", &enemy_->GetWorldTransformArea().translate.x, -10.0f, 10.0f);
	ImGui::DragFloat3("rotate", &worldTransformBase_.rotate.x);
	ImGui::Text("%d", fireTimer_);
	ImGui::Text("%f", hp_);
	ImGui::End();

	ImGui::Begin("Sprite");
	ImGui::DragFloat("TankHp", &hp_, 1.0f);
	ImGui::End();

#ifdef USE_IMGUI

	editor_.show("TankNode");
	editor_.save("Tank");

#endif // DEBUG
};

void Tank::Draw(const ViewProjection& camera) {
	BaseCharacter::Draw(camera);

	particle_->Draw(camera);
	
	RenderCollisionBounds(worldTransformBody_, camera);
}

void Tank::NoDepthDraw(const ViewProjection& camera){
	BaseCharacter::NoDepthDraw(camera);
}

void Tank::BarrierDraw(const ViewProjection& camera)
{
	if (barrier_ || barrierThreshold_ < TankConstants::kBarrierThreshold) {
		barrierModel_->Draw(worldTransformBarrier_, camera, false);
	}
	
}

// 移動
void Tank::MoveInitialize() { 
	BaseCharacter::MoveInitialize();
	minDistance_ = TankConstants::kMinDistance;
	stanAttack_ = false;
	barrier_ = false;
	distance_ = TankConstants::kTargetDistance;
};
void Tank::MoveUpdate() {
	
	BaseCharacter::MoveUpdate();

	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kBreath && enemy_->GetBehavior() == Behavior::kAttack && mp_ >= TankConstants::kBreathMpCost) {
		state_ = CharacterState::Breath;
	}
	
	if (barrierThreshold_ < TankConstants::kBarrierThreshold) {
		barrierThreshold_ += TankConstants::kBarrierThresholdIncrement;
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
	fireTimer_ = TankConstants::kFireTimerInit;
};
void Tank::AttackUpdate() {
	--fireTimer_;

	// 追従対象からロックオン対象へのベクトル
	Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

	//Y軸の回転
	BaseCharacter::DestinationAngle(sub);

	// 敵の座標までの距離
	float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));

	// 距離条件チェック
	if (minDistance_ * distance_ <= length && !followPlayer_) {
		state_ = NextState("Attack", Output1);
		searchTarget_ = true;
	}


	isAttack_ = false;
	
	if (fireTimer_ > TankConstants::kFireTimerThreshold1) {
		velocity_ = { 0.0f,0.0f,-0.01f };
		const float kCharacterSpeed = 0.1f;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		worldTransformBase_.translate += velocity_;
	}
	else if (fireTimer_ <= TankConstants::kFireTimerThreshold2 && fireTimer_ > 0) {
		isAttack_ = true;

		worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemy_->GetWorldPosition(), 0.2f);
	}
	else if (fireTimer_ <= 0) {
		fireTimer_ = TankConstants::kFireTimerInit;
		coolTime_ = TankConstants::kCoolTimeInit;
		state_ = NextState("Attack", Output1);
	}

	BaseCharacter::AttackUpdate();
}


void Tank::UniqueInitialize(){
	mp_ -= TankConstants::kBreathMpCost;
	fireTimer_ = TankConstants::kFireTimerInit;
	stanAttack_ = false;
	barrier_ = false;
	barrierThreshold_ = 1.0f;
	distance_ = TankConstants::kTargetBareathDistance;
}
void Tank::UniqueUpdate(){
	//particle_->Update();
	if (barrierThreshold_ == 1.0f) {
		worldTransformBarrier_.translate = worldTransformBase_.translate;
	}
	
	
	// 追従対象からロックオン対象へのベクトル
	Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

	//Y軸の回転
	BaseCharacter::DestinationAngle(sub);

	// 敵の座標までの距離
	float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));

	if (length >= minDistance_ * distance_) {
		barrier_ = true;
		animation_->SetLoop(false);
		animationNumber_ = kStandby;
	}
	else {
		barrier_ = false;
		animationNumber_ = kRun;
	}
	
	//バリアを展開
	if (barrier_) {
		barrierThreshold_ -= TankConstants::kBarrierThresholdDecrement;
		
		if (enemy_->GetBehavior() != Behavior::kAttack) {
			state_ = NextState("Attack", Output1);//スキルに変えておく
			barrierThreshold_ = 0.0f;
		}
	}
	else {

		const float kSpeed = 0.04f;
		// 敵の位置から自分の位置への方向ベクトルを計算
		Vector3 direction = worldTransformBase_.translate - enemy_->GetWorldTransform().translate;

		// 方向ベクトルを反転させることで敵から遠ざかる方向に移動
		Math::Normalize(direction);   // 正規化して単位ベクトルにする
		direction *= -1.0f; // 反転して反対方向に進む

		// 速度を設定
		velocity_ = direction * kSpeed;
		worldTransformBase_.translate -= velocity_;
		worldTransformBase_.translate.y = 0;


	}


}

void Tank::BreathInitialize(){
	BaseCharacter::BreathInitialize();
}
void Tank::BreathUpdate() {
	state_ = CharacterState::Unique;
}

void Tank::ProtectInitialize()
{
	BaseCharacter::ProtectInitialize();
}
void Tank::ProtectUpdate()
{
	BaseCharacter::ProtectUpdate();
}

void Tank::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	BaseCharacter::DeadInitialize();
}
void Tank::DeadUpdate() {
	/*
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
	ImGui::End();*/
}

void Tank::Relationship() {
	BaseCharacter::Relationship();	
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

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer||
		collider->GetCollisionAttribute() == kCollisionAttributeHealer ||
		collider->GetCollisionAttribute() == kCollisionAttributeRenju) {
		OBB obb = {
			.center{collider->GetOBB().center.x + collider->GetWorldPosition().x,collider->GetOBB().center.y + collider->GetWorldPosition().y,collider->GetOBB().center.z + collider->GetWorldPosition().z},

			.orientations{
			 {Vector3{collider->GetWorldTransform().matWorld_.m[0][0],collider->GetWorldTransform().matWorld_.m[0][1],collider->GetWorldTransform().matWorld_.m[0][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[1][0],collider->GetWorldTransform().matWorld_.m[1][1],collider->GetWorldTransform().matWorld_.m[1][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[2][0],collider->GetWorldTransform().matWorld_.m[2][1],collider->GetWorldTransform().matWorld_.m[2][2]}},
			},
			.size{collider->GetOBB().size}
		};

		if (state_ != CharacterState::Unique) {
			worldTransformBase_.translate += Math::PushOutAABBOBB(worldTransformBase_.translate, GetAABB(), collider->GetWorldTransform().translate, obb) * AllyAIConstants::kCollisionPushOutFactor;
			worldTransformBase_.translate.y = 0.0f;
		}
		
	}	
}

Vector3 Tank::GetBarrierWorldPos(){
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBarrier_.matWorld_.m[3][0];
	worldPos.y = worldTransformBarrier_.matWorld_.m[3][1];
	worldPos.z = worldTransformBarrier_.matWorld_.m[3][2];
	return worldPos;
}


