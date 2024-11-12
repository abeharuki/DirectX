#include "Renju.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>


Renju::~Renju() {
	delete behaviorTree_;

};

/// <summary>
/// 初期化
/// </summary>
void Renju::Initialize(Animations* animation, std::string skillName) {
	BaseCharacter::Initialize(animation, skillName);
	worldTransformBase_.translate.x = -3.0f;
	bulletModel_.reset(Model::CreateModelFromObj("resources/Renju/cube.obj", "resources/Renju/Bullet.png"));

	
	worldTransformBase_.UpdateMatrix();
	Relationship();
	worldTransformBody_.TransferMatrix();

	worldTransformShadow_.translate = { worldTransformBase_.translate.x,0.1f,worldTransformBase_.translate.z };
	worldTransformShadow_.UpdateMatrix();

	AABB aabbSize{ .min{-0.5f,-0.0f,-0.4f},.max{0.5f,1.5f,0.4f} };
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

	editor_.load("Renju");
#ifdef _DEBUG

	editor_.show("RenjuNode");
	editor_.save("Renju");

#endif // DEBUG
	
	Relationship();
	BaseCharacter::Update();

	if (behaviorTree_) {
		behaviorTree_->Update();
	}

	hitBullet_ = false;
	for (RenjuBullet* bullet : bullets_) {
		if (bullet->IsDead()) {
			hitBullet_ = true;
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


	
	ImGui::Begin("Sprite");
	ImGui::DragFloat("RenjuHp", &hp_, 1.0f);
	ImGui::End();

	ImGui::Begin("Renju");
	ImGui::Text("animeNunber%d", animationNumber_);
	ImGui::Text("animeTime%f", animation_->GetAnimationTimer());
	ImGui::End();
};

void Renju::Draw(const ViewProjection& camera) {

	// 弾の描画
	for (RenjuBullet* bullet : bullets_) {
		bullet->Draw(camera);
	}


	BaseCharacter::Draw(camera);

	RenderCollisionBounds(worldTransformBody_, camera);

}

void Renju::NoDepthDraw(const ViewProjection& camera) {
	BaseCharacter::NoDepthDraw(camera);
}

// 移動
void Renju::MoveInitialize() {
	BaseCharacter::MoveInitialize();
};
void Renju::MoveUpdate() {
	BaseCharacter::MoveUpdate();
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
	fireTimer_ = 20;
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
		animationNumber_ = standby;//攻撃モーションをいれたら変える
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
			const float kBulletSpeed = 1.0f;

			Vector3 vector = Vector3{ enemy_->GetWorldPosition().x,enemy_->GetWorldPosition().y + 7,enemy_->GetWorldPosition().z } - worldTransformBase_.GetWorldPos();
			vector = Math::Normalize(vector);
			Vector3 velocity = kBulletSpeed * vector;

			// 弾を生成、初期化
			RenjuBullet* newBullet = new RenjuBullet();
			newBullet->Initialize(
				bulletModel_.get(), worldTransformBase_.translate, { 0.3f, 0.3f, 0.3f },
				worldTransformBase_.rotate, velocity);


			bullets_.push_back(newBullet);

			fireTimer_ = 20;

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

}
void Renju::UniqueUpdate() {

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
}

// 衝突を検出したら呼び出されるコールバック関数
void Renju::OnCollision(Collider* collider) {

	BaseCharacter::OnCollision(collider);


	if (collider->GetCollisionAttribute() == kCollisionAttributePlayer ||
		collider->GetCollisionAttribute() == kCollisionAttributeHealer ||
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


