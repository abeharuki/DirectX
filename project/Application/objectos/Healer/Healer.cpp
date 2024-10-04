#include "Healer.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>


Healer::~Healer() {
	delete behaviorTree_;

};

/// <summary>
/// 初期化
/// </summary>
void Healer::Initialize() {
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/AnimatedCube", "tex.png", "bound3.gltf"));

	model_.reset(Model::CreateModelFromObj("./resources/Healer/healer.obj", "./resources/Atlas.png"));


	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate.x = 4.0f;
	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].Initialize();
		worldTransformHp_[i].translate.y = 1.5f;
		worldTransformHp_[i].translate.x = (float(i) * 0.5f)-0.5f;
		worldTransformHp_[i].scale = { 0.5f,0.5f,0.5f };
	}
	worldTransformHead_.Initialize();
	worldTransformHead_.rotate.y = 3.14f;
	worldTransformCane_.Initialize();
	worldTransformCane_.translate = { -0.63f, 0.54f, 0.0f };
	worldTransformCane_.rotate.x = -1.56f;
	worldTransformCane_.scale = { 0.5f, 0.5f, 0.5f };
	worldTransformCollision_.Initialize();
	worldTransformCollision_.scale = { 0.27f, 0.27f, 1.0f };
	worldTransformCollision_.translate.z = 1.5f;

	for (int i = 0; i < 4; ++i) {
		magicCircle_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/mahoujin.png"));
		worldTransformMagicCircle_[i].Initialize();
		worldTransformMagicCircle_[i].translate.y = 0.1f;
		worldTransformMagicCircle_[i].rotate.x = 4.7f;
		worldTransformMagicCircle_[i].scale = { 2.0f,2.0f,2.0f };
		t_[i] = 0.8f;
	}


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformHead_.TransferMatrix();
	if (nockBack_) {
		animation_->Update(0);
	}
	
	emitter_.resize(5);
	particle_.resize(5);

	
	for (int i = 0; i < 5;++i) {
		emitter_[i] = {
		.translate{0,0,0},
		.count{50},
		.frequency{1.0f},
		.frequencyTime{0.0f},
		.scaleRange{.min{0.2f,0.2f,0.0f},.max{0.2f,0.2f,0.0f}},
		.translateRange{.min{-0.1f,-0.1f,0.f},.max{0.1f,0.1f,0.f}},
		.colorRange{.min{0.5f,1,1.0f},.max{0.5f,1,1.0f}},
		.lifeTimeRange{.min{0.1f},.max{0.2f}},
		.velocityRange{.min{-0.4f,0.1f,-0.4f},.max{0.4f,0.4f,0.4f}},
		};
		particle_[i].reset(ParticleSystem::Create("resources/particle/circle.png"));
	}
	
	emitter_[0] = {
	.translate{0,0,0},
	.count{10},
	.frequency{0.02f},
	.frequencyTime{0.0f},
	.scaleRange{.min{0.05f,0.2f,0.2f},.max{0.05f,0.5f,0.2f}},
	.translateRange{.min{-0.5f,-0.5f,-0.5f},.max{0.5f,0.5f,0.5f}},
	.colorRange{.min{0.5f,1,1.0f},.max{0.5f,1,1.0f}},
	.lifeTimeRange{.min{0.1f},.max{0.5f}},
	.velocityRange{.min{0.f,0.1f,0.f},.max{0.f,0.4f,0.0f}},
	};

	AABB aabbSize{ .min{-0.5f,-0.2f,-0.25f},.max{0.5f,0.2f,0.25f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributeHealer);
	SetCollisionMask(kCollisionMaskHealer);

	// ビヘイビアツリーの初期化
	behaviorTree_ = new BehaviorTree<Healer>(this);
	behaviorTree_->Initialize();

	
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Healer::Update() {
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

	//体力がなくなあったら強制的に死亡に状態遷移
	if (hp_ <= 0) {
		hp_ = 0;
		state_ = CharacterState::Dead;
	}

	if (behaviorTree_) {
		behaviorTree_->Update();
	}

	Relationship();
	for (int i = 0; i < 4; ++i) {
		magicCircle_[i]->SetThreshold(t_[i]);
	}
	

	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kNomal && enemy_->GetAimHealer()) {
		if (enemy_->isAttack()) {
			isHit_ = true;
			if (isHit_ != preHit_) {
				hp_ -= 10;
			}
		}
	}

	// 回転
	worldTransformBase_.rotate.y =
		Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);

	worldTransformBase_.UpdateMatrix();
	worldTransformHead_.TransferMatrix();
	worldTransformCane_.TransferMatrix();
	for (int i = 0; i < 4; ++i) {
		worldTransformMagicCircle_[i].UpdateMatrix();
	}
	
	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].TransferMatrix();
	}

	if (nockBack_) {
		animation_->SetLoop(false);
		animation_->Update(0);
	}
	
	ImGui::Begin("Sprite");
	ImGui::DragFloat("HealerHp", &hp_, 1.0f);
	ImGui::End();

	ImGui::Begin("Healer");
	particle_[3]->DebugParameter();
	ImGui::Text("%f", t_);
	ImGui::DragFloat3("translat", &worldTransformMagicCircle_[0].translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &worldTransformMagicCircle_[0].rotate.x, 0.1f);
	ImGui::DragFloat3("scale", &worldTransformMagicCircle_[0].scale.x, 0.1f);
	ImGui::DragFloat("magicCirecle", &t_[0], 0.01f);
	ImGui::End();
};

void Healer::Draw(const ViewProjection& camera) {
	//animation_->Draw(worldTransformHead_, camera,true);
	model_->Draw(worldTransformHead_, camera, true);
	for (int i = 0; i < 5; ++i) {
		particle_[i]->Draw(camera);
	}
	
	for (int i = 0; i < 4; ++i) {
		magicCircle_[i]->Draw(worldTransformMagicCircle_[i], camera, true);
	}

	
	RenderCollisionBounds(worldTransformHead_, camera);
}

// 移動
void Healer::MoveInitialize() { 
	worldTransformBase_.translate.y = 0.0f;
	velocity_ = { 0.0f,0.0f,0.0f };
	searchTarget_ = false; 
	allHeal_ = false;
	oneHeal_ = false;
	heal_ = false;
};
void Healer::MoveUpdate() {
	--coolTime;
	// プレイヤーに集合
	if (operation_ || !searchTarget_) {
		followPlayer_ = true;
		searchTarget_ = false;
	}

	// 敵を探す
	if (!operation_) {
		searchTarget_ = true;
		followPlayer_ = false;
		searchTarget(enemy_->GetWorldPosition());
	}

	if (isArea_ && searchTarget_ && enemy_->IsAreaDraw()) {
		const float kCharacterSpeed = 0.3f;
		velocity_ = Math::Normalize(velocity_);
		velocity_ = Math::Multiply(kCharacterSpeed, velocity_);
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		velocity_ = Math::TransformNormal(velocity_, rotateMatrix);
		worldTransformBase_.translate += velocity_;
	}
	IsVisibleToEnemy();

	//敵の攻撃が終わったらまたジャンプできるように設定
	if (!enemy_->isAttack()) {
		jumpCount_ = 1;
	}

	//地面をたたきつける攻撃が来たらジャンプする
	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kGround && enemy_->isAttack()) {
		//ジャンプは敵の攻撃一回に対して一回まで
		if (jumpCount_ == 1 && enemylength_ <= 36) {
			//敵との距離とimpactのサイズに応じてジャンプするタイミングをずらす

			if (enemylength_ < 5 && enemy_->GetImpactSize() < 10) {
				state_ = CharacterState::Jumping;
			}

			if (Math::isWithinRange(enemylength_, 10, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 20, 10)) {
				state_ = CharacterState::Jumping;
			}

			if (Math::isWithinRange(enemylength_, 20, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 40, 10)) {
				state_ = CharacterState::Jumping;
			}

			if (Math::isWithinRange(enemylength_, 30, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 60, 10)) {
				state_ = CharacterState::Jumping;
			}
		}

	}

	//味方の体力全員が50以下なら全体回復
	//味方の体力が誰か20以下なら回復
	if (playerHp_ <= 20 || renjuHp_ <= 20 || tankHp_ <= 20 || hp_ <= 20) {
		if(mp_ >= 10){
			oneHeal_ = true;
			state_ = CharacterState::Unique;
		}
		
	}
	else if (playerHp_ < 50 && renjuHp_ < 50 && tankHp_ < 50 && hp_ < 50) {
		if(mp_ >= 20){
			allHeal_ = true;
			state_ = CharacterState::Unique;
		}
	}

};

// ジャンプ
void Healer::JumpInitialize() {
	--jumpCount_;
	worldTransformBase_.translate.y = 0.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	velocity_.y = kJumpFirstSpeed;
};
void Healer::JumpUpdate() {
	// 移動
	worldTransformBase_.translate += velocity_;
	// 重力加速度
	const float kGravity = 0.05f;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;

 	if (worldTransformBase_.translate.y <= 0.0f) {
		// ジャンプ終了
		state_ = CharacterState::Moveing;
		velocity_.y = 0.0f;
	}
};

// ノックバック
void Healer::knockInitialize() { 
	nockTime_ = 30;
	animation_->SetAnimationTimer(0, 8.0f);
	animation_->SetpreAnimationTimer(0);
	nockBack_ = true;
};
void Healer::knockUpdate() {

	//worldTransformBase_.translate += velocity_;
	//worldTransformBase_.translate.y = 0;
	if (--nockTime_ <= 0) {
		nockBack_ = false;
		animation_->SetAnimationTimer(0, 8.0f);
		animation_->SetpreAnimationTimer(0);
		if (hp_ <= 0) {
			state_ = CharacterState::Dead;
		}
		else {
			state_ = CharacterState::Moveing;
		}
		
	}
};

//アタック
void Healer::AttackInitialize() {
	worldTransformCane_.rotate = { -1.56f,0.0f,0.0f };
	searchTarget_ = false;
};
void Healer::AttackUpdate() {
	// プレイヤーの座標までの距離
	float length = Math::Length(Math::Subract(enemyPos_, worldTransformBase_.translate));

	// 距離条件チェック
	if (minDistance_ * 2 <= length && !followPlayer_) {
		state_ = CharacterState::Moveing;
		searchTarget_ = true;
	}

	workAttack_.isAttack = false;
	++workAttack_.attackParameter_;
	uint32_t anticipationTime = 15;
	uint32_t chargeTime = anticipationTime + 10;
	uint32_t swingTime = anticipationTime + chargeTime + 5;

	if (workAttack_.attackParameter_ < anticipationTime) {
		worldTransformCane_.rotate.x -= 0.04f;
	}

	if (workAttack_.attackParameter_ >= anticipationTime &&
		workAttack_.attackParameter_ < chargeTime) {
		worldTransformCane_.rotate.x -= 0.0f;
	}

	if (workAttack_.attackParameter_ >= chargeTime && workAttack_.attackParameter_ < swingTime) {
		worldTransformCane_.rotate.x += 0.15f;
		workAttack_.isAttack = true;
	}

	// 攻撃の合計時間
	uint32_t totalTime = anticipationTime + chargeTime + swingTime;


	if (workAttack_.attackParameter_ >= swingTime && workAttack_.attackParameter_ < totalTime) {
		workAttack_.attackParameter_ = 0;
		searchTarget_ = true;
		coolTime = 60;
		state_ = CharacterState::Moveing;
	}

	if (!workAttack_.isAttack) {
		//地面をたたきつける攻撃が来たらジャンプする
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kGround && enemy_->isAttack()) {
			//ジャンプは敵の攻撃一回に対して一回まで
			if (jumpCount_ == 1 && enemylength_ <= 36) {
				//敵との距離とimpactのサイズに応じてジャンプするタイミングをずらす

				if (enemylength_ < 5 && enemy_->GetImpactSize() < 10) {
					state_ = CharacterState::Jumping;
				}

				if (Math::isWithinRange(enemylength_, 10, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 20, 10)) {
					state_ = CharacterState::Jumping;
				}

				if (Math::isWithinRange(enemylength_, 20, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 40, 10)) {
					state_ = CharacterState::Jumping;
				}

				if (Math::isWithinRange(enemylength_, 30, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 60, 10)) {
					state_ = CharacterState::Jumping;
				}
			}

		}
	}

	// プレイヤーに集合
	if (operation_) {
		state_ = CharacterState::Moveing;
		workAttack_.isAttack = false;
		followPlayer_ = true;
		searchTarget_ = false;
	}
}

void Healer::UniqueInitialize(){
	//40回復
	if (oneHeal_) {
		mp_ -= 10;
		healAmount_ = 40;
	}
	//全員20回復
	if (allHeal_) {
		mp_ -= 20;
		healAmount_ = 20;
	}
	for (int i = 0; i < 5; ++i) {
		particle_[i]->SetFrequencyTime(0.0f);
		particle_[i]->SetEmitter(emitter_[i]);
	}
	

	coolTime = 60;
	for (int i = 0; i < 4; ++i) {
		t_[i] = 0.8f;
	}
	
}
void Healer::UniqueUpdate(){

	if (allHeal_) {
		if (t_[0] > 0) {
			for (int i = 0; i < 4; ++i) {
				t_[i] -= 0.02f;
			}
		}
		else {
			for (int i = 0; i < 4; ++i) {
				t_[i] -= 0.0f;
			}
		}
	}
	else {
		if (t_[0] > 0) {
			t_[0] -= 0.02f;
			if (playerHp_ <= 20) {
				t_[1] -= 0.02f;
			}
			if (renjuHp_ <= 20) {
				t_[2] -= 0.02f;
			}
			if (tankHp_ <= 20) {
				t_[3] -= 0.02f;
			}

		}
		else {
			t_[0] = 0;
		}
	}
	--coolTime;
	particle_[0]->SetTranslate(worldTransformBase_.translate);
	particle_[0]->Update();
	particle_[1]->SetTranslate(pos[0]);//player
	particle_[2]->SetTranslate(pos[1]);//renju
	particle_[3]->SetTranslate(pos[2]);//tank
	particle_[4]->SetTranslate(worldTransformBase_.translate);//healer

	if (coolTime <= 1) {
		if (allHeal_) {
			for (int i = 0; i < 5; ++i) {
				particle_[i]->Update();
			}
		}
		else {
			if (playerHp_ <= 20) {
				particle_[1]->Update();
			}
			if (hp_ <= 20) {
				particle_[4]->Update();
			}
			if (renjuHp_ <= 20) {
				particle_[2]->Update();
			}
			if (tankHp_ <= 20) {
				particle_[3]->Update();
			}
		}
	}

	if (coolTime <= 0) {
		heal_ = true;
		state_ = CharacterState::Moveing;
		coolTime = 60;
		for (int i = 0; i < 4; ++i) {
			t_[i] = 0.8f;
		}
		
	}
}

void Healer::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	isDead_ = true;
	workAttack_.isAttack = false;
}
void Healer::DeadUpdate(){
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
		state_ = CharacterState::Moveing;
		isDead_ = false;
	}

	ImGui::Begin("revival");
	ImGui::Text("T%d", revivalCount_);
	ImGui::Text("%d", isHitPlayer_);
	ImGui::Text("%d", preHitPlayer_);
	ImGui::End();

}

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
					? std::numbers::pi_v<float> -destinationAngleY_
					: -std::numbers::pi_v<float> -destinationAngleY_;
			}
		}
		else {
			destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f
				: -std::numbers::pi_v<float> / 2.0f;
		}

		// プレイヤーの座標までの距離
		float length = Math::Length(Math::Subract(playerPos, worldTransformBase_.translate));

		// 距離条件チェック
		if (minDistance_ <= length) {
			worldTransformBase_.translate =
				Math::Lerp(worldTransformBase_.translate, playerPos, 0.02f);
			if (velocity_.y == 0.0f) {
				worldTransformBase_.translate.y = 0.0f;
			}
		}
		else {
			followPlayer_ = false;
		}
	}

}
// 敵を探す
void Healer::searchTarget(Vector3 enemyPos) {
	enemyPos_ = enemyPos;
	// の座標までの距離
	enemylength_ = Math::Length(Math::Subract(enemyPos, worldTransformBase_.translate));

	if (!followPlayer_ && searchTarget_) {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemyPos - GetWorldPosition();

		// y軸周りの回転
		if (sub.z != 0.0) {
			destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

			if (sub.z < 0.0) {
				destinationAngleY_ = (sub.x >= 0.0)
					? std::numbers::pi_v<float> -destinationAngleY_
					: -std::numbers::pi_v<float> -destinationAngleY_;
			}
		}
		else {
			destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f
				: -std::numbers::pi_v<float> / 2.0f;
		}

		// 距離条件チェック
		if (minDistance_ <= enemylength_) {
			if (state_ != CharacterState::Jumping) {
				if (enemy_->GetBehaviorAttack() != BehaviorAttack::kDash || !enemy_->IsBehaberAttack()) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemyPos, 0.02f);
				}
				if (velocity_.y == 0.0f) {
					worldTransformBase_.translate.y = 0.0f;
				}
			}
			
		}
		else {
			if (coolTime <= 0 && !isArea_) {
				if (enemy_->GetBehaviorAttack() != BehaviorAttack::kDash) {
					state_ = CharacterState::Attacking;
				}
				else {
					if (!enemy_->IsBehaberAttack()) {
						state_ = CharacterState::Attacking;
					}

				}

			}

		}
	}
}

void Healer::IsVisibleToEnemy(){
	isArea_ = false;
	float rectWidth = 6.0f; // 横幅の設定 (敵の中心から±3)
	Vector3 toEnemy = enemyPos_ - worldTransformBase_.translate;
	// 敵の視線方向を取得 (Z軸方向が前方)
	Vector3 enemyForward = {
		enemy_->GetWorldTransform().matWorld_.m[2][0],
		enemy_->GetWorldTransform().matWorld_.m[2][1],
		enemy_->GetWorldTransform().matWorld_.m[2][2]
	};
	enemyForward *= -1;

	Vector3 enemyRight = Math::Cross({ 0.0f, 1.0f, 0.0f }, enemyForward);
	enemyRight = Math::Normalize(enemyRight);

	//敵との距離
	float distance = Math::Length(toEnemy);


	//距離条件のチェック
	if (enemyMinDistance_ <= distance && distance <= enemyMaxDistance_) {
		toEnemy = Math::Normalize(toEnemy); // toEnemyベクトルを正規化
		enemyForward = Math::Normalize(enemyForward); // enemyForwardベクトルを正規化

		float dot = Math::Dot(toEnemy, enemyForward);
		float angle = std::acos(dot);

		//角度条件チェック
		if (std::abs(angle) <= angleRange_) {
			if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
				RunAway();
				isArea_ = true;
			}

		}
	}
}

void Healer::RunAway(){
	if (enemyPos_.z > worldTransformBase_.translate.z) {
		if (enemyPos_.x > worldTransformBase_.translate.x) {
			velocity_ = { -1.0f,0.0f,-1.5f };
		}
		else {
			velocity_ = { 1.0f,0.0f,-1.5f };
		}
	}
	else {
		if (enemyPos_.x < worldTransformBase_.translate.x) {
			velocity_ = { -1.0f,0.0f,-1.5f };
		}
		else {
			velocity_ = { 1.0f,0.0f,-1.5f };
		}
	}
}

// 親子関係
void Healer::Relationship() {

	worldTransformHead_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformHead_.scale, worldTransformHead_.rotate, worldTransformHead_.translate),
		worldTransformBase_.matWorld_);

	worldTransformCane_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformCane_.scale, worldTransformCane_.rotate,
			worldTransformCane_.translate),
		worldTransformBase_.matWorld_);

	worldTransformMagicCircle_[0].translate.x = worldTransformBase_.translate.x;
	worldTransformMagicCircle_[0].translate.z = worldTransformBase_.translate.z;

	worldTransformMagicCircle_[1].translate.x = pos[0].x;
	worldTransformMagicCircle_[1].translate.z = pos[0].z;
	worldTransformMagicCircle_[2].translate.x = pos[1].x;
	worldTransformMagicCircle_[2].translate.z = pos[1].z;
	worldTransformMagicCircle_[3].translate.x = pos[2].x;
	worldTransformMagicCircle_[3].translate.z = pos[2].z;


	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].matWorld_ = Math::MakeScaleMatrix(worldTransformHp_[i].scale) * billboardMatrix * Math::MakeTranslateMatrix(Vector3(worldTransformBase_.translate.x + worldTransformHp_[i].translate.x, worldTransformBase_.translate.y + worldTransformHp_[i].translate.y, worldTransformBase_.translate.z));

	}


	worldTransformCollision_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformCollision_.scale, worldTransformCollision_.rotate,
			worldTransformCollision_.translate),
		worldTransformCane_.matWorld_);
}

// 衝突を検出したら呼び出されるコールバック関数
void Healer::OnAllyCollision(const WorldTransform& worldTransform) {

}
void Healer::OnCollision(const WorldTransform& worldTransform) {
	const float kSpeed = 3.0f;
	//velocity_ = { 0.0f, 0.0f, -kSpeed };
	//velocity_ = Math::TransformNormal(velocity_, worldTransform.matWorld_);
	if (hp_ > 0) {
		//behaviorRequest_ = Behavior::knock;
	}
	isHit_ = true;

	if (isHit_ != preHit_) {
		hp_ -= 10;

	}

};


void Healer::OnCollision(Collider* collider) {

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy ) {
		if (!followPlayer_ && searchTarget_) {
			if (coolTime <= 0) {
				state_ = CharacterState::Attacking;
			}
		}
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash ) {
			if (enemy_->isAttack()) {
				const float kSpeed = 3.0f;
				//velocity_ = { 0.0f, 0.0f, -kSpeed };
				//velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
				if (hp_ > 0) {
					//behaviorRequest_ = Behavior::knock;
				}
				isHit_ = true;

				if (isHit_ != preHit_) {
					if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
						hp_ -= 10.0f;
					}
					else {
						hp_ -= 5.0f;
					}

				}

			}
		}
		
		OBB obb = {
			.center{collider->GetOBB().center.x + collider->GetWorldPosition().x,collider->GetOBB().center.y + collider->GetWorldPosition().y,collider->GetOBB().center.z + collider->GetWorldPosition().z},

			.orientations{
			 {Vector3{collider->GetWorldTransform().matWorld_.m[0][0],collider->GetWorldTransform().matWorld_.m[0][1],collider->GetWorldTransform().matWorld_.m[0][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[1][0],collider->GetWorldTransform().matWorld_.m[1][1],collider->GetWorldTransform().matWorld_.m[1][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[2][0],collider->GetWorldTransform().matWorld_.m[2][1],collider->GetWorldTransform().matWorld_.m[2][2]}},
			},
			.size{collider->GetOBB().size}
		};

		worldTransformBase_.translate += Math::PushOutAABBOBB(worldTransformBase_.translate, GetAABB(), collider->GetWorldTransform().translate, obb);
	}
	
	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemyAttack) {
		if (enemy_->isAttack()) {
			const float kSpeed = 3.0f;
			//velocity_ = { 0.0f, 0.0f, -kSpeed };
			//velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
			if (hp_ > 0) {
				//behaviorRequest_ = Behavior::knock;
			}
			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= 20;

			}

		}
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributeLoderWall) {
		OBB obb = {
			.center{collider->GetOBB().center.x + collider->GetWorldPosition().x,collider->GetOBB().center.y + collider->GetWorldPosition().y,collider->GetOBB().center.z + collider->GetWorldPosition().z},

			.orientations{
			 {Vector3{collider->GetWorldTransform().matWorld_.m[0][0],collider->GetWorldTransform().matWorld_.m[0][1],collider->GetWorldTransform().matWorld_.m[0][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[1][0],collider->GetWorldTransform().matWorld_.m[1][1],collider->GetWorldTransform().matWorld_.m[1][2]}},
			 {Vector3{collider->GetWorldTransform().matWorld_.m[2][0],collider->GetWorldTransform().matWorld_.m[2][1],collider->GetWorldTransform().matWorld_.m[2][2]}},
			},
			.size{collider->GetOBB().size}
		};
		worldTransformBase_.translate += Math::PushOutAABBOBB(worldTransformBase_.translate, GetAABB(), collider->GetWorldTransform().translate, obb);
	}
	worldTransformBase_.UpdateMatrix();

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer ||
		collider->GetCollisionAttribute() == kCollisionAttributeRenju ||
		collider->GetCollisionAttribute() == kCollisionAttributeTank) {
		const float kSpeed = 0.01f;
		float subX = collider->GetWorldTransform().matWorld_.m[3][0] - GetWorldPosition().x;
		float subZ = collider->GetWorldTransform().matWorld_.m[3][2] - GetWorldPosition().z;
		if (subX < 0) {
			allyVelocity.x = kSpeed;
		}
		else {
			allyVelocity.x = -kSpeed;
		}

		if (subZ < 0) {
			allyVelocity.z = kSpeed;
		}
		else {
			allyVelocity.z = -kSpeed;
		}

		//allyVelocity = Math::TransformNormal(allyVelocity, collider->GetWorldTransform().matWorld_);
		worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, allyVelocity);
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		isHitPlayer_ = true;
	}
}


const Vector3 Healer::GetWorldPosition() const {
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