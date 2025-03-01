#include "Renju.h"
#include <CollisionManager/CollisionConfig.h>
#include <ParticleManager.h>


Renju::~Renju() {
	delete behaviorTree_;

};

/// <summary>
/// 初期化
/// </summary>
void Renju::Initialize(Animations* animation, std::string skillName) {
	AllyAICharacter::Initialize(animation, skillName);
	worldTransformBow_.Initialize();
	worldTransformArrow_.Initialize();
	worldTransformPawaGeji_.Initialize();
	worldTransformBow_.rotate = RenjuConstants::kBowRotate;
	worldTransformBase_.translate = RenjuConstants::kBaseTranslat;
	worldTransformPawaGeji_.translate = RenjuConstants::kPawaTranslat;
	worldTransformPawaGeji_.scale = { 0.0f,0.0f,0.0f };
	bulletModel_.reset(Model::CreateModelFromObj("resources/Renju/arrow.obj", "resources/Renju/bow.png"));
	bowModel_.reset(Model::CreateModelFromObj("resources/Renju/bow.obj", "resources/Renju/bow.png"));
	pawaGejiModel_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/Renju/pawa-ge-ji-0.png"));

	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();

	worldTransformShadow_.translate = { worldTransformBase_.translate.x,AllyAIConstants::kShadowTranslateOffset,worldTransformBase_.translate.z };
	worldTransformShadow_.UpdateMatrix();


	//フィールドとエミッターの設定
	emitter_ = {
		.translate = {0,3,0},
		.count{50},
		.frequency{0.075f},
		.frequencyTime{0.5f},
		.endAlpha{1},
		.scaleRange{.min{0.5f,0.5f,0.5f},.max{0.5f,0.5f,0.5f}},
		.translateRange{.min{-2.f,-1.f,-2.f},.max{2.f,2.f,2.f}},
		.colorRange{.min{0.2f,0,1.f},.max{0.2f,0,1.0f}},
		.alphaRange{.min{1.0f},.max{1.0f}},
		.lifeTimeRange{.min{0.1f},.max{1.0f}},
		.velocityRange{.min{0.0f,0.f,0.0f},.max{0.0f,0.f,0.f}},

	};

	filed_ = {
		.strength{1.f},
		.stopDistance{0.5f},
	};

	particle_ = ParticleManager::Create("resources/particle/circle.png");

	AABB aabbSize{ .min{-0.7f,-0.0f,-0.4f},.max{0.7f,1.5f,0.4f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributeRenju);
	SetCollisionMask(kCollisionMaskRenju);


	// ビヘイビアツリーの初期化
	behaviorTree_ = new BehaviorTree<Renju>(this);
	behaviorTree_->Initialize();

	distance_ = RenjuConstants::kTargetDistance;
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Renju::Update() {
	editor_.load("Renju");
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

	Relationship();

	AllyAICharacter::Update();

	worldTransformBow_.TransferMatrix();
	worldTransformArrow_.TransferMatrix();
	worldTransformPawaGeji_.TransferMatrix();

	//ゲージスプライトのアニメーション
	
	pawaGejiModel_->SetTexture("Renju/pawa-ge-ji-" + std::to_string(gejiNum_) + ".png");

	

	hitBullet_ = false;
	for (RenjuBullet* bullet : bullets_) {
		if (!bullet->IsDraw()) {
			hitBullet_ = true;
			drawWepon_ = false;
			/*particle_->StopParticle();*/
		}
		else {
			emitter_.translate = bullet->GetWorldPosition();
			emitter_.count = 3;
			emitter_.velocityRange = { .min{0,0,0},.max{0.1f,0.1f,0.1f} };
			emitter_.scaleRange = { .min{1.f,1.f,1.f},.max{1.f,1.f,1.f} };

			particle_->SetEmitter(emitter_);
			particle_->SetGravityFiled(filed_);
			particle_->Update();
		}
	}



	// デスフラグが立った弾を削除
	bullets_.remove_if([](RenjuBullet* bullet) {
		if (bullet->IsDead()) {

			delete bullet;
			return true;
		}
		return false;
	});

	for (RenjuBullet* bullet : bullets_) {

		bullet->Update();
	}

	if (behaviorTree_) {
		behaviorTree_->Update();
	}


	

	ImGui::Begin("Sprite");
	ImGui::DragFloat("RenjuHp", &hp_, 1.0f);
	ImGui::End();
	

#ifdef USE_IMGUI

	editor_.show("RenjuNode");
	editor_.save("Renju");

#endif // DEBUG
};

void Renju::Draw(const ViewProjection& camera) {

	// 弾の描画
	for (RenjuBullet* bullet : bullets_) {
		bullet->Draw(camera);
	}


	AllyAICharacter::Draw(camera);
	if (state_ == CharacterState::Attacking || state_ == CharacterState::Unique) {
		bulletModel_->Draw(worldTransformArrow_, camera, true);
	}

	if (GetDrawWepon()) {
		bowModel_->Draw(GetWorldTransformBow(), camera, false);
	}

	if (skill_) {
		particle_->Draw(camera);
	}


	RenderCollisionBounds(worldTransformBody_, camera);

}

void Renju::NoDepthDraw(const ViewProjection& camera) {
	AllyAICharacter::NoDepthDraw(camera);
	pawaGejiModel_->Draw(worldTransformPawaGeji_, camera, false);
}

// 移動
void Renju::MoveInitialize() {
	AllyAICharacter::MoveInitialize();
	minDistance_ = RenjuConstants::kMinDistance;
};
void Renju::MoveUpdate() {


	if (enemy_->GetBehavior() != Behavior::kAttack) {
		special_ = false;
	}

	if (worldTransformPawaGeji_.scale.x != 0) {
		worldTransformPawaGeji_.scale = Math::Lerp(worldTransformPawaGeji_.scale, { 0.0f,0.0f,0.0f }, RenjuConstants::kGejiScaleSpeed);
	}

	AllyAICharacter::MoveUpdate();

	//スキル
	if (mp_ >= RenjuConstants::kSkillMpCost && enemy_->IsBehaviorRequest() && coolTime_ < RenjuConstants::kCoolTimeSkill) {
		if (enemy_->GetBehavior() != enemy_->GetBehaviorRequest() && enemy_->GetBehaviorRequest() != Behavior::kAttack && enemy_->GetBehaviorAttack() != BehaviorAttack::kHenchman) {
			if (enemylength_ >= minDistance_ * RenjuConstants::kMinDistanceMultiplier) {
				state_ = CharacterState::Unique;
				special_ = false;
			}
		}
	}

	
	//子分を出す攻撃かどうか
	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman && enemy_->GetBehavior() == Behavior::kAttack && !special_ && mp_ >=RenjuConstants::kProtectMpCost) {
		state_ = CharacterState::Protect;
	}

	

};

// ジャンプ
void Renju::JumpInitialize() {
	AllyAICharacter::JumpInitialize();
};
void Renju::JumpUpdate() {
	AllyAICharacter::JumpUpdate();
};

// 攻撃
void Renju::AttackInitialize() {
	AllyAICharacter::AttackInitialize();
	skill_ = false;
	special_ = false;
	fireTimer_ = RenjuConstants::kAttackInitFireTimer;
	flameTime_ = RenjuConstants::kAttackInitFlameTime;
	worldTransformArrow_.rotate = RenjuConstants::kAttackArrowRotate;
};
void Renju::AttackUpdate() {
	
	// 距離条件チェック
	if (minDistance_ * distance_ <= enemylength_ && !followPlayer_) {
		state_ = NextState("Attack", Output1);
		searchTarget_ = true;
	}

	if (enemylength_ >= minDistance_ * RenjuConstants::kMinDistanceMultiplier) {
		isAttack_ = true;
		animation_->SetLoop(false);
		animationNumber_ = kAnimeAttack;
	}
	else {
		isAttack_ = false;
		animationNumber_ = kRun;
	}

	if (isAttack_) {
		--fireTimer_;

		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

		// y軸周りの回転
		Math::DestinationAngle(destinationAngleY_, sub);


		if (fireTimer_ == 0) {
			// 弾の速度
			const float kBulletSpeed = 1.5f;

			Vector3 vector = Vector3{ enemy_->GetWorldPosition().x,enemy_->GetWorldPosition().y + RenjuConstants::kBulletYOffset,enemy_->GetWorldPosition().z } - worldTransformBase_.GetWorldPos();
			vector = Math::Normalize(vector);
			Vector3 velocity = kBulletSpeed * vector;

			// 弾を生成、初期化
			RenjuBullet* newBullet = new RenjuBullet();
			newBullet->Initialize(
				bulletModel_.get(), worldTransformBase_.translate, RenjuConstants::kBulletScale,
				{ RenjuConstants::kBulletRotationX,worldTransformBase_.rotate.y,0.f }, velocity, skill_);


			bullets_.push_back(newBullet);

			coolTime_ = RenjuConstants::kCoolTime;
			state_ = NextState("Attack", Output1);
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


	AllyAICharacter::AttackUpdate();
}

void Renju::UniqueInitialize() {
	skill_ = true;
	drawWepon_ = true;
	isAttack_ = true;
	mp_ -= RenjuConstants::kSkillMpCost;
	animation_->SetLoop(false);
	fireTimer_ = RenjuConstants::kUniqueInitFireTimer;
	flameTime_ = RenjuConstants::kUniqueInitFlameTime;
	specialTimer_ = RenjuConstants::kSpecialTimerDuration;
	emitter_.velocityRange = { .min{0,0,0},.max{0.f,0.f,0.f} };
	emitter_.scaleRange = { .min{0.5f,0.5f,0.5f},.max{0.5f,0.5f,0.5f} };
	filed_.strength = 1.f;
	animationNumber_ = kAnimeAttack;
	
}
void Renju::UniqueUpdate() {
	//ローカル座標
	emitter_.translate = { worldTransformBow_.matWorld_.m[3][0],worldTransformBow_.matWorld_.m[3][1] ,worldTransformBow_.matWorld_.m[3][2] };

	if (isAttack_) {
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman && enemy_->IsAttack()) {
			special_ = false;
		}

		--fireTimer_;

		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

		// y軸周りの回転
		Math::DestinationAngle(destinationAngleY_, sub);

		//チャージ中アニメーションを止める
		if (fireTimer_ >= 1 && fireTimer_ <= RenjuConstants::kFireTimerCharge) {
			if (special_ && specialTimer_ > 0) {
				--specialTimer_;
				gejiNum_ = (RenjuConstants::kSpecialTimerDuration-specialTimer_) * RenjuConstants::kGejiSpriteMaxNum /RenjuConstants::kSpecialTimerDuration;
				gejiNum_ = std::min(gejiNum_, RenjuConstants::kGejiSpriteMaxNum - 1);
				if (gejiNum_ == RenjuConstants::kGejiSpriteMaxNum - 1) {
					specialTimer_ = 0;
				}
				fireTimer_ = RenjuConstants::kFireTimerCharge;
			}

			animation_->Stop(true);
			filed_.min = emitter_.translateRange.min;
			filed_.max = emitter_.translateRange.max;
			filed_.translate = emitter_.translate;
			emitter_.count = 10;//particleCount_;
			emitter_.isScaleChanging = true;//scaleFlag_;
			particle_->SetEmitter(emitter_);
			particle_->SetGravityFiled(filed_);
			particle_->Update();
		}

		

		//チャージが終わったら
		if (fireTimer_ <= 0) {

			animation_->Stop(false);
			// 弾の速度
			const float kBulletSpeed = 3.f;

			Vector3 vector = Vector3{ enemy_->GetWorldPosition().x,enemy_->GetWorldPosition().y + RenjuConstants::kBulletYOffset,enemy_->GetWorldPosition().z } - worldTransformBase_.GetWorldPos();
			vector = Math::Normalize(vector);
			Vector3 velocity = kBulletSpeed * vector;

			// 弾を生成、初期化
			RenjuBullet* newBullet = new RenjuBullet();
			newBullet->Initialize(
				bulletModel_.get(), worldTransformBase_.translate, RenjuConstants::kBulletScale,
				{ RenjuConstants::kBulletRotationX,worldTransformBase_.rotate.y,0.f }, velocity, skill_);


			bullets_.push_back(newBullet);

			//元に戻して状態遷移
			coolTime_ = RenjuConstants::kCoolTime;
			filed_.strength = 0.0f;
			particle_->StopParticle();
			state_ = NextState("Skill", Output1);
		}
	}

}

//ブレス攻撃の回避
void Renju::BreathInitialize() {
	AllyAICharacter::BreathInitialize();
}
void Renju::BreathUpdate() {
	AllyAICharacter::BreathUpdate();
}

void Renju::ProtectInitialize(){
	AllyAICharacter::ProtectInitialize();
	coolTime_ = RenjuConstants::kProtectCoolTime;
	worldTransformPawaGeji_.scale = { 0.0f,0.0f,0.0f };
	gejiNum_ = 0;
}
void Renju::ProtectUpdate(){
	--coolTime_;

	if (coolTime_ > 0) {
		worldTransformPawaGeji_.scale = Math::Lerp(worldTransformPawaGeji_.scale, RenjuConstants::kGejiMaxScale, RenjuConstants::kGejiScaleSpeed);

		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = enemy_->GetWorldPosition() - GetWorldPosition();

		// y軸周りの回転
		Math::DestinationAngle(destinationAngleY_, sub);

		const float kSpeed = 0.025f;
		// 敵の位置から自分の位置への方向ベクトルを計算
		Vector3 direction = enemy_->GetWorldTransform().translate - worldTransformBase_.translate;

		// 方向ベクトルを反転させることで敵から遠ざかる方向に移動
		Math::Normalize(direction);   // 正規化して単位ベクトルにする
		direction *= -1.0f; // 反転して反対方向に進む

		Vector3 randPos = { 0.5f,0.f,0.f };
		Matrix4x4 rotateMatrix = Math::MakeRotateYMatrix(worldTransformBase_.rotate.y);
		randPos = Math::TransformNormal(randPos, rotateMatrix);

		// 速度を設定
		velocity_ = (direction + randPos_) * kSpeed;

		worldTransformBase_.translate += velocity_;
	}
	else {
		//相手のバリアを壊すときのスキルは通常スキルの二倍のMPが必要になるのでここでもMP消費
		mp_ -= RenjuConstants::kSkillMpCost;
		special_ = true;
		state_ = CharacterState::Unique;
	}
	

	
}

void Renju::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	AllyAICharacter::DeadInitialize();
}
void Renju::DeadUpdate() {
	particle_->StopParticle();
}

void Renju::Relationship() {
	AllyAICharacter::Relationship();

	worldTransformBow_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformBow_.scale, worldTransformBow_.rotate,
			worldTransformBow_.translate),
		animation_->GetJointWorldTransform("mixamorig:LeftHand").matWorld_);

	worldTransformArrow_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformArrow_.scale, worldTransformArrow_.rotate,
			worldTransformArrow_.translate),
		animation_->GetJointWorldTransform("mixamorig:RightHand").matWorld_);

	//パワーゲージ時のビルボード
	Matrix4x4 billboardMatrixPawa = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
	billboardMatrixPawa.m[3][0] = worldTransformBase_.translate.x;
	billboardMatrixPawa.m[3][1] = worldTransformPawaGeji_.translate.y;
	billboardMatrixPawa.m[3][2] = worldTransformBase_.translate.z;
	worldTransformPawaGeji_.matWorld_ = Math::MakeScaleMatrix(worldTransformPawaGeji_.scale) * billboardMatrixPawa;
}

// 衝突を検出したら呼び出されるコールバック関数
void Renju::OnCollision(Collider* collider) {

	AllyAICharacter::OnCollision(collider);

	if (collider->GetCollisionAttribute() == kCollisionAttributeHenchman) {
		isHit_ = true;

		if (isHit_ != preHit_ && specialTimer_ > 0) {
			if (enemy_->GetBehavior() == Behavior::kAttack && enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman) {
				hp_ -= RenjuConstants::kEnemyDamageHenchman;
				specialTimer_ += RenjuConstants::kSpecialTimerIncrement;
				AllyAICharacter::DameageInit();
				damageModel_->SetTexture("character/10.png");
			}
		}
	}

	

}


