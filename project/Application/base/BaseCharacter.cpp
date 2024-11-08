#include "BaseCharacter.h"
#include <cassert>
#include <numbers>

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
		if (minDistance_ <= enemylength_) {
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


