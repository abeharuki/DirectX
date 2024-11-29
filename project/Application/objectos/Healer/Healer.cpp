#include "Healer.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>
#include <ParticleManager.h>


Healer::~Healer() {
	delete behaviorTree_;

};

/// <summary>
/// 初期化
/// </summary>
void Healer::Initialize(Animations* animation, std::string skillName) {

	// 初期化
	BaseCharacter::Initialize(animation, skillName);

	worldTransformBase_.translate.x = 6.0f;

	worldTransformCane_.Initialize();
	worldTransformCane_.translate = { -0.03f, 0.04f, 0.1f };
	worldTransformCane_.rotate = { 1.3f, 0.f, 0.f };
	worldTransformCane_.scale = { 1.f, 1.f, 1.f };



	for (int i = 0; i < 4; ++i) {
		magicCircle_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Healer/mahoujin.png"));
		healModel_[i].reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/character/H20.png"));
		worldTransformMagicCircle_[i].Initialize();
		worldTransformHeal_[i].Initialize();
		worldTransformMagicCircle_[i].translate.y = 0.11f;
		worldTransformMagicCircle_[i].rotate.x = -1.571f;
		worldTransformMagicCircle_[i].scale = { 2.0f,2.0f,2.0f };
		worldTransformHeal_[i].scale = { 0.5f,0.5f,0.5f };
		t_[i] = 0.8f;
	}


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();

	worldTransformShadow_.translate = { worldTransformBase_.translate.x,0.1f,worldTransformBase_.translate.z };
	worldTransformShadow_.UpdateMatrix();

	emitter_.resize(5);
	particle_.resize(5);


	for (int i = 0; i < 5; ++i) {
		emitter_[i] = {
		.translate{0,0,0},
		.count{50},
		.frequency{1.0f},
		.frequencyTime{0.0f},
		.scaleRange{.min{0.2f,0.2f,0.0f},.max{0.2f,0.2f,0.0f}},
		.translateRange{.min{-0.1f,-0.1f,0.f},.max{0.1f,0.1f,0.f}},
		.colorRange{.min{0.5f,1,1.0f},.max{0.5f,1,1.0f}},
		.alphaRange{.min{1.0f},.max{1.0f}},
		.lifeTimeRange{.min{0.1f},.max{0.2f}},
		.velocityRange{.min{-0.4f,0.1f,-0.4f},.max{0.4f,0.4f,0.4f}},
		};
		particle_[i] = ParticleManager::Create("resources/particle/circle.png", 10 + (i + 1));
	}

	emitter_[0] = {
	.translate{0,0,0},
	.count{10},
	.frequency{0.02f},
	.frequencyTime{0.0f},
	.scaleRange{.min{0.05f,0.2f,0.2f},.max{0.05f,0.5f,0.2f}},
	.translateRange{.min{-0.5f,-0.5f,-0.5f},.max{0.5f,0.5f,0.5f}},
	.colorRange{.min{0.5f,1,1.0f},.max{0.5f,1,1.0f}},
	.alphaRange{.min{1.0f},.max{1.0f}},
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
	//ノードの更新
	editor_.load("Healer");
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




	for (int i = 0; i < 4; ++i) {
		magicCircle_[i]->SetThreshold(t_[i]);
	}

	//回復の表示
	for (int i = 0; i < 4; ++i) {
		healModel_[i]->SetColor({ 1.f,1.f,1.f,healAlph_[i] });
		if (healAlph_[i] > 0.0f) {
			healAlph_[i] -= 0.08f;
			worldTransformHeal_[i].translate = Math::Lerp(worldTransformHeal_[i].translate, { healNumMove_[i] }, 0.05f);
		}
	}

	Relationship();
	BaseCharacter::Update();
	worldTransformCane_.TransferMatrix();
	for (int i = 0; i < 4; ++i) {
		worldTransformMagicCircle_[i].UpdateMatrix();
		worldTransformHeal_[i].TransferMatrix();
	}


	if (behaviorTree_) {
		behaviorTree_->Update();
	}


	ImGui::Begin("Sprite");
	ImGui::DragFloat("HealerHp", &hp_, 1.0f);
	ImGui::End();

	ImGui::Begin("Healer");
	particle_[3]->DebugParameter();
	ImGui::Text("%f", t_);
	ImGui::DragFloat3("translat", &worldTransformBase_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &worldTransformCane_.rotate.x, 0.1f);
	ImGui::DragFloat3("scale", &worldTransformCane_.scale.x, 0.1f);
	ImGui::DragFloat("magicCirecle", &t_[0], 0.01f);
	ImGui::End();

#ifdef _DEBUG

	editor_.show("HealerNode");
	editor_.save("Healer");
#endif // DEBUG


};

void Healer::Draw(const ViewProjection& camera) {
	BaseCharacter::Draw(camera);
	for (int i = 0; i < 5; ++i) {
		particle_[i]->Draw(camera);
	}

	for (int i = 0; i < 4; ++i) {
		magicCircle_[i]->Draw(worldTransformMagicCircle_[i], camera, true);

	}

	RenderCollisionBounds(worldTransformBody_, camera);
}

void Healer::NoDepthDraw(const ViewProjection& camera) {
	for (int i = 0; i < 4; ++i) {
		healModel_[i]->Draw(worldTransformHeal_[i], camera, false);
	}
	BaseCharacter::NoDepthDraw(camera);

};


// 移動
void Healer::MoveInitialize() {
	BaseCharacter::MoveInitialize();
	allHeal_ = false;
	oneHeal_ = false;
	heal_ = false;
};
void Healer::MoveUpdate() {

	if (!barrier_) {
		BaseCharacter::MoveUpdate();

		//味方の体力全員が50以下なら全体回復
		//味方の体力が誰か20以下なら回復
		if ((playerHp_ <= 20 && playerHp_ > 0) || (renjuHp_ <= 20 && renjuHp_ > 0) || (tankHp_ <= 20 && tankHp_ > 0) || (hp_ <= 20 && hp_ > 0)) {
			if (mp_ >= 10) {
				oneHeal_ = true;
				state_ = NextState("Move", Output3);
			}

		}
		else if (playerHp_ < 50 && renjuHp_ < 50 && tankHp_ < 50 && hp_ < 50) {
			if (mp_ >= 20) {
				allHeal_ = true;
				state_ = NextState("Move", Output3);
			}
		}
	}
	else {
		TankRunAway();
	}
	
};

// ジャンプ
void Healer::JumpInitialize() {
	BaseCharacter::JumpInitialize();
};
void Healer::JumpUpdate() {
	BaseCharacter::JumpUpdate();
};

//アタック
void Healer::AttackInitialize() {
	BaseCharacter::AttackInitialize();
};
void Healer::AttackUpdate() {
	// プレイヤーの座標までの距離
	float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));

	// 距離条件チェック
	if (minDistance_ * 2 <= length && !followPlayer_) {
		state_ = NextState("Attack", Output1);
		searchTarget_ = true;
	}

	isAttack_ = false;
	++workAttack_.attackParameter_;
	uint32_t anticipationTime = 15;
	uint32_t chargeTime = anticipationTime + 10;
	uint32_t swingTime = anticipationTime + chargeTime + 5;

	if (workAttack_.attackParameter_ >= chargeTime && workAttack_.attackParameter_ < swingTime) {
		isAttack_ = true;
	}

	// 攻撃の合計時間
	uint32_t totalTime = anticipationTime + chargeTime + swingTime;

	//攻撃が終わったら
	if (workAttack_.attackParameter_ >= swingTime && workAttack_.attackParameter_ < totalTime) {
		workAttack_.attackParameter_ = 0;
		searchTarget_ = true;
		coolTime_ = 60;
		state_ = NextState("Attack", Output1);
	}

	BaseCharacter::AttackUpdate();
}

void Healer::UniqueInitialize() {
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


	coolTime_ = 60;
	for (int i = 0; i < 4; ++i) {
		t_[i] = 0.8f;
	}

	for (int i = 0; i < 4; ++i) {
		worldTransformMagicCircle_[i].scale = { 2.f,2.f,2.f };
		worldTransformHeal_[i].scale = { 0.f,0.f,0.f };
	}

}
void Healer::UniqueUpdate() {

	// 敵の座標までの距離
	float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));

	//距離チェック
	if (length >= minDistance_ * 2.f) {
		healAnimation_ = true;
		animationNumber_ = standby;//攻撃モーションをいれたら変える
	}
	else {
		healAnimation_ = false;
		animationNumber_ = run;
	}

	if (healAnimation_) {
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
		--coolTime_;
		particle_[0]->SetTranslate(worldTransformBase_.translate);
		particle_[0]->Update();
		particle_[1]->SetTranslate(pos[0]);//player
		particle_[2]->SetTranslate(pos[1]);//renju
		particle_[3]->SetTranslate(pos[2]);//tank
		particle_[4]->SetTranslate(worldTransformBase_.translate);//healer
	}
	else {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

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


	//回復数値の設定
	worldTransformHeal_[player].translate = { pos[0].x,pos[0].y + 2.0f,pos[0].z };
	healNumMove_[player] = { worldTransformHeal_[player].translate.x ,worldTransformHeal_[player].translate.y + 2.0f,worldTransformHeal_[player].translate.z };

	worldTransformHeal_[renju].translate = { pos[1].x,pos[1].y + 2.0f,pos[1].z };
	healNumMove_[renju] = { worldTransformHeal_[renju].translate.x ,worldTransformHeal_[renju].translate.y + 2.0f,worldTransformHeal_[renju].translate.z };

	worldTransformHeal_[tank].translate = { pos[2].x,pos[2].y + 2.0f,pos[2].z };
	healNumMove_[tank] = { worldTransformHeal_[tank].translate.x ,worldTransformHeal_[tank].translate.y + 2.0f,worldTransformHeal_[tank].translate.z };

	worldTransformHeal_[healer].translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
	healNumMove_[healer] = { worldTransformHeal_[healer].translate.x ,worldTransformHeal_[healer].translate.y + 2.0f,worldTransformHeal_[healer].translate.z };




	if (coolTime_ < 5) {
		for (int i = 0; i < 4; ++i) {
			worldTransformMagicCircle_[i].scale = worldTransformMagicCircle_[i].scale - 1.0f;
		}
	}
	else if (coolTime_ < 10) {
		for (int i = 0; i < 4; ++i) {
			worldTransformMagicCircle_[i].scale = worldTransformMagicCircle_[i].scale + 0.5f;
		}
	}


	if (coolTime_ <= 1) {
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

	if (coolTime_ <= 0) {
		heal_ = true;
		state_ = NextState("Heal", Output1);
		coolTime_ = 60;

		for (int i = 0; i < 4; ++i) {
			t_[i] = 0.8f;
			healAlph_[i] = 2.0f;
		}

	}
}

void Healer::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	BaseCharacter::DeadInitialize();
}
void Healer::DeadUpdate() {
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

void Healer::TankRunAway()
{


	if (barrier_) {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = pos[2] - GetWorldPosition();

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

		const float kSpeed = 0.06f;
		// 敵の位置から自分の位置への方向ベクトルを計算
		Vector3 direction = pos[2] - enemy_->GetWorldTransform().translate;

		// 方向ベクトルを反転させることで敵から遠ざかる方向に移動
		Math::Normalize(direction);   // 正規化して単位ベクトルにする
		direction *= -1.0f; // 反転して反対方向に進む

		// 速度を設定
		velocity_ = direction * kSpeed;

		if (Math::isWithinRange(worldTransformBase_.translate.x, (pos[2].x - (velocity_.x * 5.0f)), 2.0f) &&
			Math::isWithinRange(worldTransformBase_.translate.z,(pos[2].z - (velocity_.z * 5.0f)),2.0f)) {
			animationNumber_ = standby;
		}
		else {
			animationNumber_ = run;
			worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, pos[2] - (velocity_ * 5.0f), 0.05f);
			worldTransformBase_.translate.y = 0;
		}

	}
	



}

// 親子関係
void Healer::Relationship() {

	BaseCharacter::Relationship();

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


	for (int i = 0; i < 4; ++i) {
		Matrix4x4 billboardMatrixHealNum = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
		billboardMatrixHealNum.m[3][0] = worldTransformHeal_[i].translate.x;
		billboardMatrixHealNum.m[3][1] = worldTransformHeal_[i].translate.y;
		billboardMatrixHealNum.m[3][2] = worldTransformHeal_[i].translate.z;
		worldTransformHeal_[i].matWorld_ = Math::MakeScaleMatrix(worldTransformHeal_[i].scale) * billboardMatrixHealNum;

	}

}

// 衝突を検出したら呼び出されるコールバック関数
void Healer::OnCollision(Collider* collider) {

	BaseCharacter::OnCollision(collider);

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		if (!followPlayer_ && searchTarget_) {
			if (coolTime_ <= 0) {
				state_ = CharacterState::Attacking;
			}
		}
	}

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



