#include "Renju.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>
#include <ParticleManager.h>


Renju::~Renju() {
	delete behaviorTree_;

};

/// <summary>
/// 初期化
/// </summary>
void Renju::Initialize(Animations* animation, std::string skillName) {
	BaseCharacter::Initialize(animation, skillName);
	worldTransformBow_.Initialize();
	worldTransformArrow_.Initialize();
	worldTransformBow_.rotate = { 1.9f,0.0f,-1.5f };
	worldTransformBase_.translate.x = -3.0f;
	bulletModel_.reset(Model::CreateModelFromObj("resources/Renju/arrow.obj", "resources/Renju/bow.png"));
	bowModel_.reset(Model::CreateModelFromObj("resources/Renju/bow.obj", "resources/Renju/bow.png"));


	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();

	worldTransformShadow_.translate = { worldTransformBase_.translate.x,0.1f,worldTransformBase_.translate.z };
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

	particle_ = ParticleManager::Create("resources/particle/circle.png", 9);

	AABB aabbSize{ .min{-0.7f,-0.0f,-0.4f},.max{0.7f,1.5f,0.4f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributeRenju);
	SetCollisionMask(kCollisionMaskRenju);


	// ビヘイビアツリーの初期化
	behaviorTree_ = new BehaviorTree<Renju>(this);
	behaviorTree_->Initialize();

	distance_ = 2;
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

	preHitPlayer_ = isHitPlayer_;
	isHitPlayer_ = false;



	Relationship();

	BaseCharacter::Update();

	worldTransformBow_.TransferMatrix();
	worldTransformArrow_.TransferMatrix();




	

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

	ImGui::Begin("Renju");
	ImGui::Text("animeNunber%d", animationNumber_);
	ImGui::Text("animeTime%f", animation_->GetAnimationTimer());
	ImGui::SliderFloat3("BowPos", &worldTransformArrow_.translate.x, -10.0f, 10.0f);
	ImGui::DragFloat3("rotate", &worldTransformArrow_.rotate.x, 0.1f);

	if (ImGui::TreeNode("Particle")) {

		ImGui::SliderInt("ParticelCount", &particleCount_, 1, 50);
		ImGui::SliderFloat("Frequency", &emitter_.frequency, 0.0f, 5.0f);
		ImGui::DragFloat3("Pos", &emitter_.translate.x, 0.1f);
		ImGui::DragFloat3("PosRangeMin", &emitter_.translateRange.min.x, 0.1f);
		ImGui::DragFloat3("PosRangeMax", &emitter_.translateRange.max.x, 0.1f);
		ImGui::DragFloat3("ScaleRangeMin", &emitter_.scaleRange.min.x, 0.01f);
		ImGui::DragFloat3("ScaleRangeMax", &emitter_.scaleRange.max.x, 0.01f);

		ImGui::SliderFloat3("ColorMin", &emitter_.colorRange.min.x, 0.0f, 1.0f);
		ImGui::SliderFloat3("ColorMax", &emitter_.colorRange.max.x, 0.0f, 1.0f);
		ImGui::SliderFloat("AlphaMin", &emitter_.alphaRange.min, 0.0f, 10.0f);
		ImGui::SliderFloat("AlphaMax", &emitter_.alphaRange.max, 0.0f, 10.0f);
		ImGui::SliderFloat("lifeTimeMin", &emitter_.lifeTimeRange.min, 0.0f, 1.0f);
		ImGui::SliderFloat("lifeTimeMax", &emitter_.lifeTimeRange.max, 0.0f, 1.0f);
		ImGui::DragFloat3("VelocityMin", &emitter_.velocityRange.min.x, 0.1f);
		ImGui::DragFloat3("VelocityMax", &emitter_.velocityRange.max.x, 0.1f);
		ImGui::Checkbox("scaleFlag", &scaleFlag_);
		ImGui::SliderFloat("endAlpha", &emitter_.endAlpha, 0.0f, 10.0f);
		ImGui::SliderFloat3("scaleAlpha", &emitter_.endScale.x, 0.0f, 3.0f);

		ImGui::SliderFloat("GravityStrength", &filed_.strength, 0.0f, 10.0f);
		ImGui::SliderFloat("GravityStop", &filed_.stopDistance, 0.0f, 10.0f);

		ImGui::TreePop();
	}

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


	BaseCharacter::Draw(camera);
	if (state_ == CharacterState::Attacking || state_ == CharacterState::Unique) {
		bulletModel_->Draw(worldTransformArrow_, camera, true);
	}

	if (GetDrawWepon()) {
		bowModel_->Draw(GetWorldTransformBow(), camera, true);
	}

	if (skill_) {
		particle_->Draw(camera);
	}


	RenderCollisionBounds(worldTransformBody_, camera);

}

void Renju::NoDepthDraw(const ViewProjection& camera) {
	BaseCharacter::NoDepthDraw(camera);
}

// 移動
void Renju::MoveInitialize() {
	BaseCharacter::MoveInitialize();
	minDistance_ = 10.0f;
};
void Renju::MoveUpdate() {


	if (enemy_->GetBehavior() != Behavior::kAttack) {
		special_ = false;
	}

	BaseCharacter::MoveUpdate();

	//スキル
	if (mp_ >= 10 && enemy_->IsBehaviorRequest() && coolTime_ < 10.f) {
		if (enemy_->GetBehavior() != enemy_->GetBehaviorRequest() && enemy_->GetBehaviorRequest() != Behavior::kAttack && enemy_->GetBehaviorAttack() != BehaviorAttack::kHenchman) {
			// 敵の座標までの距離
			float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));
			if (length >= minDistance_ * 1.5f) {
				state_ = CharacterState::Unique;
				special_ = false;
			}
		}
	}

	
	//子分を出す攻撃かどうか
	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman && enemy_->GetBehavior() == Behavior::kAttack && !special_ && mp_ >=20) {
		float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));
		if (length >= minDistance_ * 3.f) {
			state_ = CharacterState::Protect;
		}
		else {
			const float kSpeed = 0.06f;
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

	

};

// ジャンプ
void Renju::JumpInitialize() {
	BaseCharacter::JumpInitialize();
};
void Renju::JumpUpdate() {
	BaseCharacter::JumpUpdate();
};

// 攻撃
void Renju::AttackInitialize() {
	BaseCharacter::AttackInitialize();
	skill_ = false;
	special_ = false;
	fireTimer_ = 20;
	flameTime_ = 60.0f;
	worldTransformArrow_.rotate = { -0.3f,0.0f,1.3f };
};
void Renju::AttackUpdate() {
	// 敵の座標までの距離
	float length = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));

	// 距離条件チェック
	if (minDistance_ * 2 <= length && !followPlayer_) {
		state_ = NextState("Attack", Output1);
		searchTarget_ = true;
	}

	if (length >= minDistance_ * 1.5f) {
		isAttack_ = true;
		animation_->SetLoop(false);
		animationNumber_ = animeAttack;
	}
	else {
		isAttack_ = false;
		animationNumber_ = run;
	}

	if (isAttack_) {
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


		if (fireTimer_ == 0) {
			// 弾の速度
			const float kBulletSpeed = 1.5f;

			Vector3 vector = Vector3{ enemy_->GetWorldPosition().x,enemy_->GetWorldPosition().y + 7,enemy_->GetWorldPosition().z } - worldTransformBase_.GetWorldPos();
			vector = Math::Normalize(vector);
			Vector3 velocity = kBulletSpeed * vector;

			// 弾を生成、初期化
			RenjuBullet* newBullet = new RenjuBullet();
			newBullet->Initialize(
				bulletModel_.get(), worldTransformBase_.translate, { 1.5f, 1.5f, 1.5f },
				{ 1.2f,worldTransformBase_.rotate.y,0.f }, velocity, skill_);


			bullets_.push_back(newBullet);

			coolTime_ = 60;
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


	BaseCharacter::AttackUpdate();
}

void Renju::UniqueInitialize() {
	skill_ = true;
	drawWepon_ = true;
	isAttack_ = true;
	mp_ -= 10;
	animation_->SetLoop(false);
	fireTimer_ = 60;
	flameTime_ = 60.0f;
	specialTimer_ = 60 * 7;
	emitter_.velocityRange = { .min{0,0,0},.max{0.f,0.f,0.f} };
	emitter_.scaleRange = { .min{0.5f,0.5f,0.5f},.max{0.5f,0.5f,0.5f} };
	filed_.strength = 1.f;
	animationNumber_ = animeAttack;
}
void Renju::UniqueUpdate() {
	emitter_.translate = { worldTransformBow_.matWorld_.m[3][0],worldTransformBow_.matWorld_.m[3][1] ,worldTransformBow_.matWorld_.m[3][2] };

	if (isAttack_) {
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman && enemy_->isAttack()) {
			special_ = false;
		}

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

		//チャージ中アニメーションを止める
		if (fireTimer_ >= 1 && fireTimer_ <= 48) {
			if (special_ && specialTimer_ >= 0) {
				--specialTimer_;
				fireTimer_ = 48;
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

			Vector3 vector = Vector3{ enemy_->GetWorldPosition().x,enemy_->GetWorldPosition().y + 7,enemy_->GetWorldPosition().z } - worldTransformBase_.GetWorldPos();
			vector = Math::Normalize(vector);
			Vector3 velocity = kBulletSpeed * vector;

			// 弾を生成、初期化
			RenjuBullet* newBullet = new RenjuBullet();
			newBullet->Initialize(
				bulletModel_.get(), worldTransformBase_.translate, { 1.5f, 1.5f, 1.5f },
				{ 1.2f,worldTransformBase_.rotate.y,0.f }, velocity, skill_);


			bullets_.push_back(newBullet);

			//元に戻して状態遷移
			coolTime_ = 60;
			filed_.strength = 0.0f;
			particle_->StopParticle();
			state_ = NextState("Skill", Output1);
		}
	}

}

//ブレス攻撃の回避
void Renju::BreathInitialize() {
	BaseCharacter::BreathInitialize();
}
void Renju::BreathUpdate() {
	BaseCharacter::BreathUpdate();
}

void Renju::ProtectInitialize(){
	BaseCharacter::ProtectInitialize();
	coolTime_ = 30;
}
void Renju::ProtectUpdate(){
	--coolTime_;

	if (coolTime_ > 0) {
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

		const float kSpeed = 0.001f;
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
		mp_ -= 10;
		special_ = true;
		state_ = CharacterState::Unique;
	}
	

	
}

void Renju::DeadInitialize() {
	//復活時間
	revivalCount_ = 0;
	BaseCharacter::DeadInitialize();
}
void Renju::DeadUpdate() {
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

void Renju::Relationship() {
	BaseCharacter::Relationship();

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

}

// 衝突を検出したら呼び出されるコールバック関数
void Renju::OnCollision(Collider* collider) {

	BaseCharacter::OnCollision(collider);

	if (collider->GetCollisionAttribute() == kCollisionAttributeHenchman) {
		isHit_ = true;

		if (isHit_ != preHit_) {
			if (enemy_->GetBehavior() == Behavior::kAttack && enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman) {
				hp_ -= 10.0f;
				alpha_ = 2.0f;
				specialTimer_ += 30;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/10.png");
			}
		}
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer||
		collider->GetCollisionAttribute() == kCollisionAttributeHealer||
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
			worldTransformBase_.translate += Math::PushOutAABBOBB(worldTransformBase_.translate, GetAABB(), collider->GetWorldTransform().translate, obb) * 0.3f;
			worldTransformBase_.translate.y = 0.0f;
		}
	}

	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer) {
		isHitPlayer_ = true;
	}
}


