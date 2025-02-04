#include "Healer.h"
#include <ParticleManager.h>


Healer::~Healer() {
	delete behaviorTree_;

};

/// <summary>
/// 初期化
/// </summary>
void Healer::Initialize(Animations* animation, std::string skillName) {

	// 初期化
	AllyAICharacter::Initialize(animation, skillName);

	worldTransformBase_.translate = HealerConstants::kBaseTranslate;

	worldTransformCane_.Initialize();
	worldTransformCane_.translate = HealerConstants::kCaneTranslate;
	worldTransformCane_.rotate = HealerConstants::kCaneRotate;
	worldTransformCane_.scale = HealerConstants::kCaneScale;



	for (int i = 0; i < kMaxCharacterNum; ++i) {
		magicCircle_[i].reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/Healer/mahoujin.png"));
		healModel_[i].reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/character/H20.png"));
		worldTransformMagicCircle_[i].Initialize();
		worldTransformHeal_[i].Initialize();
		worldTransformMagicCircle_[i].translate = HealerConstants::kMagicCircleTranslate;
		worldTransformMagicCircle_[i].rotate = HealerConstants::kMagicCircleRotate;
		worldTransformMagicCircle_[i].scale = HealerConstants::kMagicCircleScale;
		worldTransformHeal_[i].scale = HealerConstants::kHealScale;
		threshold_[i] = HealerConstants::kMagicThreshold;
	}


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();

	worldTransformShadow_.translate = { worldTransformBase_.translate.x,AllyAIConstants::kShadowTranslateOffset,worldTransformBase_.translate.z };
	worldTransformShadow_.UpdateMatrix();

	emitter_.resize(5);
	particle_.resize(5);


	for (int i = 0; i < HealerConstants::kEmitterCount; ++i) {
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
		particle_[i] = ParticleManager::Create("resources/particle/circle.png");
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

	AABB aabbSize{ .min{-0.7f,-0.0f,-0.4f},.max{0.7f,1.5f,0.4f} };
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



	//魔法陣の表示
	for (int i = 0; i < kMaxCharacterNum; ++i) {
		magicCircle_[i]->SetThreshold(threshold_[i]);
	}

	//回復数値の表示
	for (int i = 0; i < kMaxCharacterNum; ++i) {
		healModel_[i]->SetColor({ 1.f,1.f,1.f,healAlph_[i] });
		if (healAlph_[i] > 0.0f) {
			healAlph_[i] -= HealerConstants::kHealAlphaDecrement;
			worldTransformHeal_[i].translate = Math::Lerp(worldTransformHeal_[i].translate, { healNumMove_[i] }, 0.05f);
		}
	}

	Relationship();
	AllyAICharacter::Update();
	worldTransformCane_.TransferMatrix();
	for (int i = 0; i < kMaxCharacterNum; ++i) {
		worldTransformMagicCircle_[i].UpdateMatrix();
		worldTransformHeal_[i].TransferMatrix();
	}


	if (behaviorTree_) {
		behaviorTree_->Update();
	}


	ImGui::Begin("Sprite");
	ImGui::DragFloat("HealerHp", &hp_, 1.0f);
	ImGui::End();
#ifdef USE_IMGUI

	editor_.show("HealerNode");
	editor_.save("Healer");
#endif // DEBUG


};

void Healer::Draw(const ViewProjection& camera) {
	AllyAICharacter::Draw(camera);
	for (int i = 0; i < HealerConstants::kEmitterCount; ++i) {
		particle_[i]->Draw(camera);
	}

	for (int i = 0; i < kMaxCharacterNum; ++i) {
		magicCircle_[i]->Draw(worldTransformMagicCircle_[i], camera, true);
	}

	RenderCollisionBounds(worldTransformBody_, camera);
}

void Healer::NoDepthDraw(const ViewProjection& camera) {
	//回復数値の描画
	for (int i = 0; i < 4; ++i) {
		healModel_[i]->Draw(worldTransformHeal_[i], camera, false);
	}
	AllyAICharacter::NoDepthDraw(camera);

};

// 移動
void Healer::MoveInitialize() {
	AllyAICharacter::MoveInitialize();
	allHeal_ = false;
	oneHeal_ = false;
	heal_ = false;
};
void Healer::MoveUpdate() {

	AllyAICharacter::MoveUpdate();


	//味方の体力全員が50以下なら全体回復
	//味方の体力が誰か30以下なら回復

	if ((playerHp_ <= HealerConstants::kSingleHealHpThreshold && playerHp_ > 0) || (renjuHp_ <= HealerConstants::kSingleHealHpThreshold && renjuHp_ > 0) || (tankHp_ <= HealerConstants::kSingleHealHpThreshold && tankHp_ > 0) || (hp_ <= HealerConstants::kSingleHealHpThreshold && hp_ > 0)) {
		if (mp_ >= HealerConstants::kSingleHealMpCost && coolTime_ <= HealerConstants::kCoolTimeThreshold) {
			oneHeal_ = true;
			state_ = NextState("Move", Output3);
		}

	}
	else if (playerHp_ <= HealerConstants::kAllHealHpThreshold && renjuHp_ <= HealerConstants::kAllHealHpThreshold && tankHp_ <= HealerConstants::kAllHealHpThreshold && hp_ <= HealerConstants::kAllHealHpThreshold) {
		if (mp_ >= HealerConstants::kAllHealMpCost && coolTime_ <= HealerConstants::kCoolTimeThreshold) {
			allHeal_ = true;
			state_ = NextState("Move", Output3);
		}
	}



};

// ジャンプ
void Healer::JumpInitialize() {
	AllyAICharacter::JumpInitialize();
};
void Healer::JumpUpdate() {
	AllyAICharacter::JumpUpdate();
};

//アタック
void Healer::AttackInitialize() {
	AllyAICharacter::AttackInitialize();
	emitter_[kHealer] = {
	.translate{0,0,0},
	.count{50},
	.frequency{1000.f},
	.frequencyTime{0.0f},
	.scaleRange{.min{1.0f,1.f,1.f},.max{1.f,1.f,1.f}},
	.translateRange{.min{0.f,0.f,0.f},.max{0.f,0.f,0.f}},
	.colorRange{.min{0.3f,0,0.3f},.max{0.5f,0,1.0f}},
	.alphaRange{.min{1.0f},.max{1.0f}},
	.lifeTimeRange{.min{0.1f},.max{0.2f}},
	.velocityRange{.min{-0.7f,-0.7f,-0.7f},.max{0.7f,0.7f,0.7f}},
	};
	emitter_[kHealer].isScaleChanging = true;
	particle_[kHealer]->SetFrequencyTime(0.0f);
	particle_[kHealer]->SetEmitter(emitter_[kHealer]);
};
void Healer::AttackUpdate() {
	
	// 距離条件チェック
	if (enemy_->GetBehaviorAttack() != BehaviorAttack::kHenchman) {
		if (minDistance_ * HealerConstants::kMinDistanceMultiplier <= enemylength_ && !followPlayer_) {
			state_ = NextState("Attack", Output1);
			searchTarget_ = true;
		}
	}
	

	isAttack_ = false;
	++attackParameter_;
	if (attackParameter_ >= HealerConstants::kAttackStartFrames) {
		isAttack_ = true;
		if (currentTarget_) {
			if (!currentTarget_->IsDead()) {
				particle_[kHealer]->SetTranslate({ currentTarget_->GetWorldPosition().x,currentTarget_->GetWorldPosition().y + 1.0f,currentTarget_->GetWorldPosition().z });
				currentTarget_->SetDamaged(true);
				particle_[kHealer]->Update();
			}
			else {
				particle_[kHealer]->StopParticle();
			}
		}
	}
	

	//攻撃が終わったら
	if (attackParameter_ >= HealerConstants::kAttackEndFrames) {
		attackParameter_ = 0;
		searchTarget_ = true;
		coolTime_ = HealerConstants::kAttackCoolTime;
		state_ = NextState("Attack", Output1);
	}

	AllyAICharacter::AttackUpdate();

}

void Healer::UniqueInitialize() {
	//40回復
	if (oneHeal_) {
		mp_ -= HealerConstants::kSingleHealMpCost;
		healAmount_ = HealerConstants::kSingleHealAmount;
		for (int i = 0; i < kMaxCharacterNum; ++i) {
			healModel_[i]->SetTexture("character/H40.png");
		}

		for (int i = 0; i < kMaxCharacterNum; ++i) {
			worldTransformMagicCircle_[i].scale = HealerConstants::kMagicCircleScale;
		}

		if (hp_ <= HealerConstants::kHealThreshold) {
			worldTransformHeal_[kHealer].scale = { 0.f,0.f,0.f };
		}else
		if (playerHp_ <= HealerConstants::kHealThreshold) {
			worldTransformHeal_[kPlayer].scale = { 0.f,0.f,0.f };
		}
		else
		if (renjuHp_ <= HealerConstants::kHealThreshold) {
			worldTransformHeal_[kRenju].scale = { 0.f,0.f,0.f };
		}
		else
		if (tankHp_ <= HealerConstants::kHealThreshold) {
			worldTransformHeal_[kTank].scale = { 0.f,0.f,0.f };
		}

	}
	//全員20回復
	if (allHeal_) {
		mp_ -= HealerConstants::kAllHealMpCost;
		healAmount_ = HealerConstants::kAllHealAmount;
		for (int i = 0; i < kMaxCharacterNum; ++i) {
			healModel_[i]->SetTexture("character/H20.png");
			worldTransformHeal_[i].scale = { 0.f,0.f,0.f };
		}

	}

	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman) {
		mp_ -= HealerConstants::kSingleHealMpCost;
		healAmount_ = HealerConstants::kSingleHealAmount;
		for (int i = 0; i < kMaxCharacterNum; ++i) {
			healModel_[i]->SetTexture("character/H40.png");
		}

		for (int i = 0; i < kMaxCharacterNum; ++i) {
			worldTransformMagicCircle_[i].scale = HealerConstants::kMagicCircleScale;
		}

		if (hp_ <= HealerConstants::kHenchmanHealThreshold) {
			worldTransformHeal_[kHealer].scale = { 0.f,0.f,0.f };
		}
		else
		if (playerHp_ <= HealerConstants::kHenchmanHealThreshold) {
			worldTransformHeal_[kPlayer].scale = { 0.f,0.f,0.f };
		}
		else
		if (renjuHp_ <= HealerConstants::kHenchmanHealThreshold) {
			worldTransformHeal_[kRenju].scale = { 0.f,0.f,0.f };
		}
		else
		if (tankHp_ <= HealerConstants::kHenchmanHealThreshold) {
			worldTransformHeal_[kTank].scale = { 0.f,0.f,0.f };
		}
	}


	emitter_[kHealer] = {
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
	emitter_[kHealer].isScaleChanging = true;
	for (int i = 0; i < HealerConstants::kEmitterCount; ++i) {
		particle_[i]->SetFrequencyTime(0.0f);
		particle_[i]->SetEmitter(emitter_[i]);
	}


	coolTime_ = HealerConstants::kCoolTimeReset;
	for (int i = 0; i < kMaxCharacterNum; ++i) {
		threshold_[i] = HealerConstants::kMagicThreshold;
	}



}
void Healer::UniqueUpdate() {

	//距離チェック
	if (enemylength_ >= minDistance_ * HealerConstants::kMinDistanceMultiplier) {
		healAnimation_ = true;
		animationNumber_ = kStandby;//攻撃モーションをいれたら変える
	}
	else {
		if (!healAnimation_) {
			healAnimation_ = false;
			animationNumber_ = kRun;
		}

	}

	//全体ヒールかどうか
	if (healAnimation_) {
		if (allHeal_) {
			if (threshold_[kHealer] > 0) {
				for (int i = 0; i < kMaxCharacterNum; ++i) {
					threshold_[i] -= HealerConstants::kThresholdDecrease;
					worldTransformHeal_[i].scale = HealerConstants::kHealAnimationScale;
				}
			}
			else {
				for (int i = 0; i < kMaxCharacterNum; ++i) {
					threshold_[i] = 0.0f;
				}
			}
		}
		else {
			if (threshold_[kHealer] > 0) {
				threshold_[kHealer] -= HealerConstants::kThresholdDecrease;
				if (hp_ <= HealerConstants::kHealThreshold && hp_ > 0) {
					worldTransformHeal_[kHealer].scale = HealerConstants::kHealAnimationScale;
				}
				else
				if (playerHp_ <= HealerConstants::kHealThreshold && playerHp_ > 0) {
					threshold_[kPlayer] -= HealerConstants::kThresholdDecrease;
					worldTransformHeal_[kPlayer].scale = HealerConstants::kHealAnimationScale;
				}
				else
				if (renjuHp_ <= HealerConstants::kHealThreshold && renjuHp_ > 0) {
					threshold_[kRenju] -= HealerConstants::kThresholdDecrease;
					worldTransformHeal_[kRenju].scale = HealerConstants::kHealAnimationScale;
				}
				else
				if (tankHp_ <= HealerConstants::kHealThreshold && tankHp_ > 0) {
					threshold_[kTank] -= HealerConstants::kThresholdDecrease;
					worldTransformHeal_[kTank].scale = HealerConstants::kHealAnimationScale;
				}

			}
			else {
				threshold_[kHealer] = 0;
			}
		}

		//子分が出ているときのヒール
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman) {
			if (threshold_[kHealer] > 0) {
				threshold_[kHealer] -= HealerConstants::kThresholdDecrease;
				if (hp_ <= HealerConstants::kHealThreshold && hp_ > 0) {
					worldTransformHeal_[kHealer].scale = HealerConstants::kHealAnimationScale;
				}
				else
				if (playerHp_ <= HealerConstants::kHenchmanHealThreshold && playerHp_ > 0) {
					threshold_[kPlayer] -= HealerConstants::kThresholdDecrease;
					worldTransformHeal_[kPlayer].scale = HealerConstants::kHealAnimationScale;
				}
				else
				if (renjuHp_ <= HealerConstants::kHenchmanHealThreshold && renjuHp_ > 0) {
					threshold_[kRenju] -= HealerConstants::kThresholdDecrease;
					worldTransformHeal_[kRenju].scale = HealerConstants::kHealAnimationScale;
				}
				else
				if (tankHp_ <= HealerConstants::kHenchmanHealThreshold && tankHp_ > 0) {
					threshold_[kTank] -= HealerConstants::kThresholdDecrease;
					worldTransformHeal_[kTank].scale = HealerConstants::kHealAnimationScale;
				}

			}
			else {
				threshold_[kHealer] = 0;
			}
		}

		--coolTime_;
		particle_[kHealer]->SetTranslate(worldTransformBase_.translate);
		particle_[kHealer]->Update();
		particle_[kPlayer]->SetTranslate(playerPos_);//player
		particle_[kRenju]->SetTranslate(renjuPos_);//renju
		particle_[kTank]->SetTranslate(tankPos_);//tank
		particle_[kMaxCharacterNum]->SetTranslate(worldTransformBase_.translate);//healer
	}
	else {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

		// y軸周りの回転
		Math::DestinationAngle(destinationAngleY_, sub);


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
	if (hp_ <= HealerConstants::kHealThreshold && hp_ > 0) {
		worldTransformHeal_[kHealer].translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + HealerConstants::kHealPositionYOffset,worldTransformBase_.translate.z };
		healNumMove_[kHealer] = { worldTransformHeal_[kHealer].translate.x ,worldTransformHeal_[kHealer].translate.y + HealerConstants::kHealPositionYOffset,worldTransformHeal_[kHealer].translate.z };

	}
	if (playerHp_ <= HealerConstants::kHealThreshold && playerHp_ > 0) {
		worldTransformHeal_[kPlayer].translate = { playerPos_.x,playerPos_.y + HealerConstants::kHealPositionYOffset,playerPos_.z };
		healNumMove_[kPlayer] = { worldTransformHeal_[kPlayer].translate.x ,worldTransformHeal_[kPlayer].translate.y + HealerConstants::kHealPositionYOffset,worldTransformHeal_[kPlayer].translate.z };

	}
	if (renjuHp_ <= HealerConstants::kHealThreshold && renjuHp_ > 0) {
		worldTransformHeal_[kRenju].translate = { renjuPos_.x,renjuPos_.y + HealerConstants::kHealPositionYOffset,renjuPos_.z };
		healNumMove_[kRenju] = { worldTransformHeal_[kRenju].translate.x ,worldTransformHeal_[kRenju].translate.y + HealerConstants::kHealPositionYOffset,worldTransformHeal_[kRenju].translate.z };

	}
	if (tankHp_ <= HealerConstants::kHealThreshold && tankHp_ > 0) {
		worldTransformHeal_[kTank].translate = { tankPos_.x,tankPos_.y + 2.0f,tankPos_.z };
		healNumMove_[kTank] = { worldTransformHeal_[kTank].translate.x ,worldTransformHeal_[kTank].translate.y + 2.0f,worldTransformHeal_[kTank].translate.z };

	}



	if (coolTime_ < 5) {
		for (int i = 0; i < kMaxCharacterNum; ++i) {
			worldTransformMagicCircle_[i].scale = worldTransformMagicCircle_[i].scale - HealerConstants::kMagicCircleScaleDecrease;
		}
	}
	else if (coolTime_ < 10) {
		for (int i = 0; i < kMaxCharacterNum; ++i) {
			worldTransformMagicCircle_[i].scale = worldTransformMagicCircle_[i].scale + HealerConstants::kMagicCircleScaleIncrease;
		}
	}

	//パーティクルの更新
	if (coolTime_ <= 1) {
		if (allHeal_) {
			for (int i = 0; i < HealerConstants::kEmitterCount; ++i) {
				particle_[i]->Update();
			}
		}
		else {
			if (playerHp_ <= HealerConstants::kHealThreshold && playerHp_ > 0) {
				particle_[kPlayer]->Update();
			}
			else
			if (hp_ <= HealerConstants::kHealThreshold && hp_ > 0) {
				particle_[kMaxCharacterNum]->Update();//healer
			}
			else
			if (renjuHp_ <= HealerConstants::kHealThreshold && renjuHp_ > 0) {
				particle_[kRenju]->Update();
			}
			else
			if (tankHp_ <= HealerConstants::kHealThreshold && tankHp_ > 0) {
				particle_[kTank]->Update();
			}
		}

		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman) {

			if (playerHp_ <= HealerConstants::kHealThreshold && playerHp_ > 0) {
				particle_[kPlayer]->Update();
			}
			else
			if (hp_ <= HealerConstants::kHealThreshold && hp_ > 0) {
				particle_[kMaxCharacterNum]->Update();//healer
			}
			else
			if (renjuHp_ <= HealerConstants::kHealThreshold && renjuHp_ > 0) {
				particle_[kRenju]->Update();
			}
			else
			if (tankHp_ <= HealerConstants::kHealThreshold && tankHp_ > 0) {
				particle_[kTank]->Update();
			}

		}
	}

	if (coolTime_ <= 0) {
		heal_ = true;
		state_ = NextState("Heal", Output1);
		coolTime_ = HealerConstants::kCoolTimeReset;

		if (playerHp_ <= HealerConstants::kHenchmanHealThreshold && playerHp_ > 0) {
			healAlph_[kPlayer] = HealerConstants::kHealNumThreshold;
			threshold_[kPlayer] = HealerConstants::kMagicThreshold;
		}
		else
		if (hp_ <= HealerConstants::kHenchmanHealThreshold && hp_ > 0) {
			healAlph_[kHealer] = HealerConstants::kHealNumThreshold;
			threshold_[kHealer] = HealerConstants::kMagicThreshold;
		}
		else
		if (renjuHp_ <= HealerConstants::kHenchmanHealThreshold && renjuHp_ > 0) {
			healAlph_[kRenju] = HealerConstants::kHealNumThreshold;
			threshold_[kRenju] = HealerConstants::kMagicThreshold;
		}
		else
		if (tankHp_ <= HealerConstants::kHenchmanHealThreshold && tankHp_ > 0) {
			healAlph_[kTank] = HealerConstants::kHealNumThreshold;
			threshold_[kTank] = HealerConstants::kMagicThreshold;
		}

	}
}

//ブレス攻撃の回避
void Healer::BreathInitialize() {
	AllyAICharacter::BreathInitialize();
}
void Healer::BreathUpdate() {
	AllyAICharacter::BreathUpdate();
}

void Healer::ProtectInitialize() {
	AllyAICharacter::ProtectInitialize();
}
void Healer::ProtectUpdate() {
	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman) {
		if ((playerHp_ <= HealerConstants::kHenchmanHealHpThreshold && playerHp_ > 0) || (renjuHp_ <= HealerConstants::kHenchmanHealHpThreshold && renjuHp_ > 0) || (tankHp_ <= HealerConstants::kHenchmanHealHpThreshold && tankHp_ > 0) || (hp_ <= HealerConstants::kHenchmanHealHpThreshold && hp_ > 0)) {
			if (mp_ >= HealerConstants::kSingleHealMpCost) {
				state_ = CharacterState::Unique;
			}
		}
	}
	

	AllyAICharacter::ProtectUpdate();
}

void Healer::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	AllyAICharacter::DeadInitialize();
}
void Healer::DeadUpdate() {
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

// 親子関係
void Healer::Relationship() {

	AllyAICharacter::Relationship();

	worldTransformCane_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformCane_.scale, worldTransformCane_.rotate,
			worldTransformCane_.translate),
		animation_->GetJointWorldTransform("mixamorig:RightHand").matWorld_);

	worldTransformMagicCircle_[kHealer].translate.x = worldTransformBase_.translate.x;
	worldTransformMagicCircle_[kHealer].translate.z = worldTransformBase_.translate.z;

	worldTransformMagicCircle_[kPlayer].translate.x = playerPos_.x;
	worldTransformMagicCircle_[kPlayer].translate.z = playerPos_.z;
	worldTransformMagicCircle_[kRenju].translate.x = renjuPos_.x;
	worldTransformMagicCircle_[kRenju].translate.z = renjuPos_.z;
	worldTransformMagicCircle_[kTank].translate.x = tankPos_.x;
	worldTransformMagicCircle_[kTank].translate.z = tankPos_.z;


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

	AllyAICharacter::OnCollision(collider);

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		if (!followPlayer_ && searchTarget_) {
			if (coolTime_ <= 0) {
				state_ = NextState("Move", Output1);
			}
		}
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer ||
		collider->GetCollisionAttribute() == kCollisionAttributeRenju ||
		collider->GetCollisionAttribute() == kCollisionAttributeTank) {
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

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		isHitPlayer_ = true;
	}
}



