#include "BaseCharacter.h"


void BaseCharacter::Initialize(Animations* animation, std::string className){
	className_ = className;
	animation_ = animation;
	animationNumber_ = standby;
	flameTime_ = 30.0f;

	InitializePerCharacter();

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformNum_.Initialize();
	worldTransformNum_.scale = { 0.5f,0.5f,0.5f };
	damageModel_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/character/20.png"));
	alpha_ = 0.0f;

	state_ = CharacterState::Moveing;
}

void BaseCharacter::Update(){
	//体力がなくなあったら強制的に死亡に状態遷移
	if (hp_ <= 0) {
		hp_ = 0;
		if (NextState("Move", Output4) == CharacterState::Dead ||
			NextState("Attack", Output3) == CharacterState::Dead ||
			NextState("Jump", Output2) == CharacterState::Dead ||
			NextState(skillName_, Output2) == CharacterState::Dead) {
			state_ = CharacterState::Dead;
		}
	}

	//各キャラの処理にしないといけない
	if (enemy_->GetBehaviorAttack() == BehaviorAttack::kNomal && GetAimCharacter()) {
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


	// 回転
	worldTransformBase_.rotate.y =
		Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
	animation_->SetFlameTimer(flameTime_);
	animation_->Update(animationNumber_);
	worldTransformBase_.UpdateMatrix();
	worldTransformBody_.TransferMatrix();
	worldTransformNum_.TransferMatrix();
}

void BaseCharacter::Draw(const ViewProjection& camera){
	animation_->Draw(worldTransformBody_, camera, true);
}
void BaseCharacter::NoDepthDraw(const ViewProjection& camera){
	damageModel_->Draw(worldTransformNum_, camera, false);
}

void BaseCharacter::MoveInitialize()
{
	worldTransformBase_.translate.y = 0.0f;
	velocity_ = { 0.0f,0.0f,0.0f };
	searchTarget_ = false;
	isAttack_ = false;
	animation_->SetpreAnimationTimer(0);
	animation_->SetLoop(true);
	flameTime_ = 30.0f;
	animationNumber_ = standby;
}
void BaseCharacter::MoveUpdate()
{
	--coolTime_;
	// 敵の座標までの距離
	enemylength_ = Math::Length(Math::Subract(enemy_->GetWorldPosition(), worldTransformBase_.translate));


	// プレイヤーに集合
	if ((operation_ || !searchTarget_) && (!gameStart_ || battleStart_)) {
		followPlayer_ = true;
		searchTarget_ = false;
		followPlayer();
	}

	// 敵を探す
	if (!operation_) {
		searchTarget_ = true;
		followPlayer_ = false;
		searchTarget();
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
		//ジャンプは敵の攻撃一回に対して一回まで
		if (jumpCount_ == 1 && enemylength_ <= 35) {
			//敵との距離とimpactのサイズに応じてジャンプするタイミングをずらす

			if (enemylength_ < 5 && enemy_->GetImpactSize() < 10) {
				state_ = NextState("Move", Output2);
			}

			if (Math::isWithinRange(enemylength_, 10, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 20, 10)) {
				state_ = NextState("Move", Output2);
			}

			if (Math::isWithinRange(enemylength_, 20, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 40, 10)) {
				state_ = NextState("Move", Output2);
			}

			if (Math::isWithinRange(enemylength_, 30, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 60, 10)) {
				state_ = NextState("Move", Output2);
			}
		}

	}
}

void BaseCharacter::JumpInitialize()
{
	--jumpCount_;
	worldTransformBase_.translate.y = 0.0f;
	// ジャンプ初速
	const float kJumpFirstSpeed = 0.6f;
	velocity_.y = kJumpFirstSpeed;
	animationNumber_ = jump;
	flameTime_ = 30.0f;
	animation_->SetAnimationTimer(0.5f, flameTime_);
	animation_->SetLoop(false);
}
void BaseCharacter::JumpUpdate()
{
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
		state_ = NextState("Jump", Output1);
		velocity_.y = 0.0f;
	}
}

void BaseCharacter::AttackInitialize()
{
	searchTarget_ = false;
	animationNumber_ = animeAttack;
	animation_->SetLoop(false);
}
void BaseCharacter::AttackUpdate()
{
	if (!isAttack_) {
		//地面をたたきつける攻撃が来たらジャンプする
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kGround && enemy_->isAttack()) {
			//ジャンプは敵の攻撃一回に対して一回まで
			if (jumpCount_ == 1 && enemylength_ <= 36) {
				//敵との距離とimpactのサイズに応じてジャンプするタイミングをずらす

				if (enemylength_ < 5 && enemy_->GetImpactSize() < 10) {
					state_ = NextState("Attack", Output2);
				}

				if (Math::isWithinRange(enemylength_, 10, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 20, 10)) {
					state_ = NextState("Attack", Output2);
				}

				if (Math::isWithinRange(enemylength_, 20, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 40, 10)) {
					state_ = NextState("Attack", Output2);
				}

				if (Math::isWithinRange(enemylength_, 30, 5) && Math::isWithinRange(enemy_->GetImpactSize(), 60, 10)) {
					state_ = NextState("Attack", Output2);
				}
			}

		}
	}

	// プレイヤーに集合
	if (operation_) {
		state_ = NextState("Attack", Output1);
		isAttack_ = false;
		followPlayer_ = true;
		searchTarget_ = false;
		animation_->SetLoop(true);
	}
}

void BaseCharacter::DeadInitialize()
{
	isAttack_ = false;
	isDead_ = true;
	animationNumber_ = death;
	animation_->SetLoop(false);
}

void BaseCharacter::Relationship()
{
	worldTransformBody_.matWorld_ = Math::Multiply(
		Math::MakeAffineMatrix(
			worldTransformBody_.scale, worldTransformBody_.rotate, worldTransformBody_.translate),
		worldTransformBase_.matWorld_);

	backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrixNum = backToFrontMatrix * Math::Inverse(viewProjection_.matView);
	billboardMatrixNum.m[3][0] = worldTransformNum_.translate.x;
	billboardMatrixNum.m[3][1] = worldTransformNum_.translate.y;
	billboardMatrixNum.m[3][2] = worldTransformNum_.translate.z;
	worldTransformNum_.matWorld_ = Math::MakeScaleMatrix(worldTransformNum_.scale) * billboardMatrixNum;

}

void BaseCharacter::followPlayer()
{
	if (followPlayer_ && state_ != CharacterState::Unique) {
		// 追従対象からロックオン対象へのベクトル
		Vector3 sub = playerPos_ - GetWorldPosition();

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
		float length = Math::Length(Math::Subract(playerPos_, worldTransformBase_.translate));

		// 距離条件チェック
		if (minDistance_ <= length) {
			worldTransformBase_.translate =
				Math::Lerp(worldTransformBase_.translate, playerPos_, 0.02f);
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

void BaseCharacter::Dissolve()
{
	animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
	threshold_ += 0.004f;
	animation_->SetThreshold(threshold_);
}

void BaseCharacter::searchTarget()
{
	if (!followPlayer_ && searchTarget_) {
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

		// 距離条件チェック
		if (minDistance_ * distance_ <= enemylength_) {
			if (state_ != CharacterState::Jumping) {
				if (enemy_->GetBehaviorAttack() != BehaviorAttack::kDash || !enemy_->IsBehaberAttack()) {
					worldTransformBase_.translate = Math::Lerp(worldTransformBase_.translate, enemy_->GetWorldPosition(), 0.02f);
					animationNumber_ = run;
				}
				if (velocity_.y == 0.0f) {
					worldTransformBase_.translate.y = 0.0f;
				}
			}

		}
		else {
			animationNumber_ = standby;
			if (coolTime_ <= 0 && !isArea_ && enemy_->GetBehavior() != Behavior::kDead) {
				if (enemy_->GetBehaviorAttack() != BehaviorAttack::kDash) {
					state_ = NextState("Move", Output1);
				}
				else {
					if (!enemy_->IsBehaberAttack()) {
						state_ = NextState("Move", Output1);
					}

				}

			}

		}
	}
}

void BaseCharacter::IsVisibleToEnemy()
{
	isArea_ = false;
	//float rectWidth = 6.0f; // 横幅の設定 (敵の中心から±3)
	Vector3 toEnemy = enemy_->GetWorldPosition() - worldTransformBase_.translate;
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
			if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash && enemy_->IsAreaDraw()) {
				RunAway();
				isArea_ = true;
			}

		}
	}
}

void BaseCharacter::RunAway()
{
	animationNumber_ = run;
	if (enemy_->GetWorldPosition().z > worldTransformBase_.translate.z) {
		if (enemy_->GetWorldPosition().x > worldTransformBase_.translate.x) {
			velocity_ = { -1.0f,0.0f,-1.5f };
		}
		else {
			velocity_ = { 1.0f,0.0f,-1.5f };
		}
	}
	else {
		if (enemy_->GetWorldPosition().x < worldTransformBase_.translate.x) {
			velocity_ = { -1.0f,0.0f,-1.5f };
		}
		else {
			velocity_ = { 1.0f,0.0f,-1.5f };
		}
	}

}

CharacterState BaseCharacter::NextState(std::string name, int outputNum)
{
	auto linkedNode = editor_.GetLinkNode(name, outputNum);

	if (linkedNode.name == "Move") {
		return CharacterState::Moveing;
	}
	else if (linkedNode.name == "Attack") {
		return CharacterState::Attacking;
	}
	else if (linkedNode.name == "Jump") {
		return CharacterState::Jumping;
	}
	else if (linkedNode.name == skillName_) {
		return CharacterState::Unique;
	}
	else if (linkedNode.name == "Dead") {
		return CharacterState::Dead;
	}
	else {
		return CharacterState::Moveing; // デフォルトの状態
	}
}

void BaseCharacter::InitializePerCharacter()
{
	if (className_ == "Healer") {
		skillName_ = "Heal";
	}
	else if (className_ == "Renju") {
		skillName_ = "???";
	}
	else if (className_ == "Tank") {
		skillName_ = "Stan";
	}
	
}

bool BaseCharacter::GetAimCharacter()
{
	if (className_ == "Healer") {
		return enemy_->GetAimHealer();
	}
	else if (className_ == "Renju") {
		return enemy_->GetAimRenju();
	}
	else if (className_ == "Tank") {
		return enemy_->GetAimTank();
	}
	else {
		return false;
	}
	
}



void BaseCharacter::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {
		if (enemy_->GetBehaviorAttack() == BehaviorAttack::kDash) {
			if (enemy_->isAttack()) {

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
		if (enemy_->isAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kGround) {

			isHit_ = true;

			if (isHit_ != preHit_) {
				hp_ -= 20;
				alpha_ = 2.0f;
				worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + 2.0f,worldTransformBase_.translate.z };
				numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + 2.0f,worldTransformNum_.translate.z };
				damageModel_->SetTexture("character/20.png");
			}

		}

		if (enemy_->isAttack() && enemy_->GetBehaviorAttack() == BehaviorAttack::kThrowing) {
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
}

const Vector3 BaseCharacter::GetWorldPosition() const
{
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}
