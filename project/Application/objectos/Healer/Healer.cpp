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
	animation_.reset(Animations::Create("./resources/Healer", "Atlas.png", "healer.gltf"));
	animationNumber_ = standby;
	flameTime_ = 30.0f;

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBase_.translate.x = 6.0f;
	for (int i = 0; i < 3; i++) {
		worldTransformHp_[i].Initialize();
		worldTransformHp_[i].translate.y = 1.5f;
		worldTransformHp_[i].translate.x = (float(i) * 0.5f)-0.5f;
		worldTransformHp_[i].scale = { 0.5f,0.5f,0.5f };
	}
	worldTransformHead_.Initialize();
	worldTransformCane_.Initialize();
	worldTransformCane_.translate = { -0.03f, 0.04f, 0.1f };
	worldTransformCane_.rotate = { 1.3f, 0.f, 0.f };
	worldTransformCane_.scale = { 1.f, 1.f, 1.f };
	worldTransformCollision_.Initialize();
	worldTransformCollision_.scale = { 0.27f, 0.27f, 1.0f };
	worldTransformCollision_.translate.z = 1.5f;

	worldTransformNum_.Initialize();
	worldTransformNum_.scale = { 0.5f,0.5f,0.5f };
	damageModel_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/character/20.png"));
	alpha_ = 0.0f;

	for (int i = 0; i < 4; ++i) {
		magicCircle_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/mahoujin.png"));
		healModel_[i].reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/character/H20.png"));
		worldTransformMagicCircle_[i].Initialize();
		worldTransformHeal_[i].Initialize();
		worldTransformMagicCircle_[i].translate.y = 0.1f;
		worldTransformMagicCircle_[i].rotate.x = 4.7f;
		worldTransformMagicCircle_[i].scale = { 2.0f,2.0f,2.0f };
		worldTransformHeal_[i].scale = { 0.5f,0.5f,0.5f };
		t_[i] = 0.8f;
	}


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformHead_.TransferMatrix();

	
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

	AABB aabbSize{ .min{-0.5f,-0.0f,-0.4f},.max{0.5f,1.5f,0.4f} };
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
				alpha_ = 2.0f;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/10.png");
			}
		}
	}

	//ダメージの表示
	damageModel_->SetColor({ 1.f,1.f,1.f,alpha_ });
	if (alpha_ > 0.0f) {
		alpha_ -= 0.08f;
		worldTransformNum_.translate = Math::Lerp(worldTransformNum_.translate, { numMove_ }, 0.05f);
	}

	//回復の表示
	for (int i = 0; i < 4; ++i) {
		healModel_[i]->SetColor({ 1.f,1.f,1.f,healAlph_[i]});
		if (healAlph_[i] > 0.0f) {
			healAlph_[i] -= 0.08f;
			worldTransformHeal_[i].translate = Math::Lerp(worldTransformHeal_[i].translate, {healNumMove_[i]}, 0.05f);
		}
	}
	


	// 回転
	worldTransformBase_.rotate.y =
		Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
	animation_->SetFlameTimer(flameTime_);
	animation_->Update(animationNumber_);
	worldTransformBase_.UpdateMatrix();
	worldTransformHead_.TransferMatrix();
	worldTransformCane_.TransferMatrix();
	worldTransformNum_.TransferMatrix();
	for (int i = 0; i < 4; ++i) {
		worldTransformMagicCircle_[i].UpdateMatrix();
		worldTransformHeal_[i].TransferMatrix();
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
	ImGui::DragFloat3("translat", &worldTransformCane_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &worldTransformCane_.rotate.x, 0.1f);
	ImGui::DragFloat3("scale", &worldTransformCane_.scale.x, 0.1f);
	ImGui::DragFloat("magicCirecle", &t_[0], 0.01f);
	ImGui::End();
};

void Healer::Draw(const ViewProjection& camera) {
	animation_->Draw(worldTransformHead_, camera,true);
	for (int i = 0; i < 5; ++i) {
		particle_[i]->Draw(camera);
	}
	
	for (int i = 0; i < 4; ++i) {
		magicCircle_[i]->Draw(worldTransformMagicCircle_[i], camera, true);
		
	}
	
	RenderCollisionBounds(worldTransformHead_, camera);
}

void Healer::NoDepthDraw(const ViewProjection& camera){
	for (int i = 0; i < 4; ++i) {
		healModel_[i]->Draw(worldTransformHeal_[i], camera, false);
	}
	damageModel_->Draw(worldTransformNum_, camera, false);

};


// 移動
void Healer::MoveInitialize() { 
	worldTransformBase_.translate.y = 0.0f;
	velocity_ = { 0.0f,0.0f,0.0f };
	searchTarget_ = false; 
	allHeal_ = false;
	oneHeal_ = false;
	heal_ = false;
	animation_->SetLoop(true);
	animation_->SetpreAnimationTimer(0);
	flameTime_ = 30.0f;
	animationNumber_ = standby;
};
void Healer::MoveUpdate() {
	--coolTime;
	// 敵の座標までの距離
	enemylength_ = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));


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
		animationNumber_ = run;
	}
	IsVisibleToEnemy();

	//敵の攻撃が終わったらまたジャンプできるように設定
	if (!enemy_->isAttack()) {
		jumpCount_ = 1;
	}

	//地面をたたきつける攻撃が来たらジャンプする
	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kGround && enemy_->isAttack()) {
		enemylength_ = Math::Length(Math::Subract(enemyPos_, worldTransformBase_.translate));
		//ジャンプは敵の攻撃一回に対して一回まで
		if (jumpCount_ == 1 && enemylength_ <= 36) {
			//敵との距離とimpactのサイズに応じてジャンプするタイミングをずらす

			if (Math::isWithinRange(enemylength_, 30, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 60, 10)) {
				state_ = CharacterState::Jumping;
			}
			else if (Math::isWithinRange(enemylength_, 20, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 40, 10)) {
				state_ = CharacterState::Jumping;
			}
			else if (Math::isWithinRange(enemylength_, 10, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 20, 10)) {
				state_ = CharacterState::Jumping;
			}
			else if (enemylength_ < 5 && enemy_->GetImpactSize() < 10) {
				state_ = CharacterState::Jumping;
			}
		}

	}

	//味方の体力全員が50以下なら全体回復
	//味方の体力が誰か20以下なら回復
	if ((playerHp_ <= 20 && playerHp_ > 0)|| (renjuHp_ <= 20 && renjuHp_ > 0)|| (tankHp_ <= 20 && tankHp_ > 0)|| (hp_ <= 20 && hp_ > 0)) {
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
	animationNumber_ = jump;
	flameTime_ = 30.0f;
	animation_->SetAnimationTimer(0.5f, flameTime_);
	animation_->SetLoop(false);
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
	
	searchTarget_ = false;
	animationNumber_ = animeAttack;
	animation_->SetLoop(false);
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

	if (workAttack_.attackParameter_ >= chargeTime && workAttack_.attackParameter_ < swingTime) {
		//worldTransformCane_.rotate.x += 0.15f;
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
		animation_->SetLoop(true);
	}
}

void Healer::UniqueInitialize(){
	//40回復
	if (oneHeal_) {
		mp_ -= 10;
		healAmount_ = 40;
		for (int i = 0; i < 4; ++i) {
			healModel_[i]->SetTexture("character/H40.png");
		}
		
	}
	//全員20回復
	if (allHeal_) {
		mp_ -= 20;
		healAmount_ = 20;
		for (int i = 0; i < 4; ++i) {
			healModel_[i]->SetTexture("character/H20.png");
		}

	}
	for (int i = 0; i < 5; ++i) {
		particle_[i]->SetFrequencyTime(0.0f);
		particle_[i]->SetEmitter(emitter_[i]);
	}
	

	coolTime = 60;
	for (int i = 0; i < 4; ++i) {
		t_[i] = 0.8f;
	}
	
	for (int i = 0; i < 4; ++i) {
		worldTransformMagicCircle_[i].scale = { 2.f,2.f,2.f };
		worldTransformHeal_[i].scale = { 0.f,0.f,0.f };
	}

}
void Healer::UniqueUpdate(){

	if (allHeal_) {
		if (t_[healer] > 0) {
			for (int i = 0; i < 4; ++i) {
				t_[i] -= 0.02f;
				worldTransformHeal_[i].scale = { 0.5f,0.5f,0.5f };
			}
		}
		else {
			for (int i = 0; i < 4; ++i) {
				t_[i] -= 0.0f;
			}
		}
	}
	else {
		if (t_[healer] > 0) {
			t_[healer] -= 0.02f;
			if (hp_ <= 20) {
				worldTransformHeal_[healer].scale = { 0.5f,0.5f,0.5f };
			}
			
			if (playerHp_ <= 20) {
				t_[player] -= 0.02f;
				worldTransformHeal_[player].scale = { 0.5f,0.5f,0.5f };
			}
			if (renjuHp_ <= 20) {
				t_[renju] -= 0.02f;
				worldTransformHeal_[renju].scale = { 0.5f,0.5f,0.5f };
			}
			if (tankHp_ <= 20) {
				t_[tank] -= 0.02f;
				worldTransformHeal_[tank].scale = { 0.5f,0.5f,0.5f };
			}

		}
		else {
			t_[healer] = 0;
		}
	}
	--coolTime;
	particle_[0]->SetTranslate(worldTransformBase_.translate);
	particle_[0]->Update();
	particle_[1]->SetTranslate(pos[0]);//player
	particle_[2]->SetTranslate(pos[1]);//renju
	particle_[3]->SetTranslate(pos[2]);//tank
	particle_[4]->SetTranslate(worldTransformBase_.translate);//healer

	//回復数値の設定
	worldTransformHeal_[player].translate = {pos[0].x,pos[0].y + 2.0f,pos[0].z};
	healNumMove_[player] = { worldTransformHeal_[player].translate.x ,worldTransformHeal_[player].translate.y + 2.0f,worldTransformHeal_[player].translate.z };

	worldTransformHeal_[renju].translate = { pos[1].x,pos[1].y + 2.0f,pos[1].z };
	healNumMove_[renju] = { worldTransformHeal_[renju].translate.x ,worldTransformHeal_[renju].translate.y + 2.0f,worldTransformHeal_[renju].translate.z };

	worldTransformHeal_[tank].translate = { pos[2].x,pos[2].y + 2.0f,pos[2].z };
	healNumMove_[tank] = { worldTransformHeal_[tank].translate.x ,worldTransformHeal_[tank].translate.y + 2.0f,worldTransformHeal_[tank].translate.z };

	worldTransformHeal_[healer].translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
	healNumMove_[healer] = {worldTransformHeal_[healer].translate.x ,worldTransformHeal_[healer].translate.y + 2.0f,worldTransformHeal_[healer].translate.z};




	if (coolTime < 5) {
		for (int i = 0; i < 4; ++i) {
			worldTransformMagicCircle_[i].scale = worldTransformMagicCircle_[i].scale - 1.0f;
		}
	}else if (coolTime < 10) {
		for (int i = 0; i < 4; ++i) {
			worldTransformMagicCircle_[i].scale = worldTransformMagicCircle_[i].scale + 0.5f;
		}
	}


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
			healAlph_[i] = 2.0f;
		}
		
	}
}

void Healer::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	isDead_ = true;
	workAttack_.isAttack = false;
	animationNumber_ = death;
	animation_->SetLoop(false);
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
			animationNumber_ = run;
			if (velocity_.y == 0.0f) {
				worldTransformBase_.translate.y = 0.0f;
			}
		}
		else {
			followPlayer_ = false;
			animationNumber_ = standby;
		}
	}

}
// 敵を探す
void Healer::searchTarget(Vector3 enemyPos) {
	enemyPos_ = enemyPos;
	
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
					animationNumber_ = run;
				}
				if (velocity_.y == 0.0f) {
					worldTransformBase_.translate.y = 0.0f;
				}
			}
			
		}
		else {
			animationNumber_ = standby;
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
	//float rectWidth = 6.0f; // 横幅の設定 (敵の中心から±3)
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
	animationNumber_ = run;
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
		animation_->GetJointWorldTransform("mixamorig:RightHand").matWorld_);

	worldTransformMagicCircle_[0].translate.x = worldTransformBase_.translate.x;
	worldTransformMagicCircle_[0].translate.z = worldTransformBase_.translate.z;

	worldTransformMagicCircle_[1].translate.x = pos[0].x;
	worldTransformMagicCircle_[1].translate.z = pos[0].z;
	worldTransformMagicCircle_[2].translate.x = pos[1].x;
	worldTransformMagicCircle_[2].translate.z = pos[1].z;
	worldTransformMagicCircle_[3].translate.x = pos[2].x;
	worldTransformMagicCircle_[3].translate.z = pos[2].z;

	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrixNum = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
	billboardMatrixNum.m[3][0] = worldTransformNum_.translate.x;
	billboardMatrixNum.m[3][1] = worldTransformNum_.translate.y;
	billboardMatrixNum.m[3][2] = worldTransformNum_.translate.z;
	worldTransformNum_.matWorld_ = Math::MakeScaleMatrix(worldTransformNum_.scale) * billboardMatrixNum;

	for (int i = 0; i < 4; ++i) {
		Matrix4x4 billboardMatrixHealNum = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
		billboardMatrixHealNum.m[3][0] = worldTransformHeal_[i].translate.x;
		billboardMatrixHealNum.m[3][1] = worldTransformHeal_[i].translate.y;
		billboardMatrixHealNum.m[3][2] = worldTransformHeal_[i].translate.z;
		worldTransformHeal_[i].matWorld_ = Math::MakeScaleMatrix(worldTransformHeal_[i].scale) * billboardMatrixHealNum;

	}

	worldTransformCollision_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformCollision_.scale, worldTransformCollision_.rotate,
			worldTransformCollision_.translate),
		worldTransformCane_.matWorld_);
}

// 衝突を検出したら呼び出されるコールバック関数
void Healer::OnAllyCollision(const WorldTransform& worldTransform) {
	worldTransform;
}
void Healer::OnCollision(const WorldTransform& worldTransform) {
	worldTransform;
	//const float kSpeed = 3.0f;
	//velocity_ = { 0.0f, 0.0f, -kSpeed };
	//velocity_ = Math::TransformNormal(velocity_, worldTransform.matWorld_);
	if (hp_ > 0) {
		//behaviorRequest_ = Behavior::knock;
	}
	isHit_ = true;

	if (isHit_ != preHit_) {
		hp_ -= 10;
		alpha_ = 2.0f;
		worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
		numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
		damageModel_->SetTexture("character/10.png");
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
				//const float kSpeed = 3.0f;
				//velocity_ = { 0.0f, 0.0f, -kSpeed };
				//velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
				if (hp_ > 0) {
					//behaviorRequest_ = Behavior::knock;
				}
				isHit_ = true;

				if (isHit_ != preHit_) {
					if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
						hp_ -= 10.0f;
						alpha_ = 2.0f;
						worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
						numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
						damageModel_->SetTexture("character/10.png");
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
			//const float kSpeed = 3.0f;
			//velocity_ = { 0.0f, 0.0f, -kSpeed };
			//velocity_ = Math::TransformNormal(velocity_, collider->GetWorldTransform().matWorld_);
			if (hp_ > 0) {
				//behaviorRequest_ = Behavior::knock;
			}
			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= 20;
				alpha_ = 2.0f;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/20.png");
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