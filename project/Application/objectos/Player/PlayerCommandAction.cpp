#include "PlayerCommandAction.h"
#include <ParticleManager.h>


void PlayerCommandAcition::Init(Animations* animation){
	animation_ = animation;
	particle_ = ParticleManager::Create("resources/particle/circle.png");
}

void PlayerCommandAcition::Update(){

	if (behaviorRequest_) {
		// 振る舞い変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化
		switch (behavior_) {
		case ActionBehavior::kNomalAttack:
		default:
			NomalAttaclInit();
			break;
		case ActionBehavior::kFireBall:
			FireBallInit();
			break;
		}

		// 振る舞いリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case ActionBehavior::kNomalAttack:
	default:
		// 通常攻撃
		NomalAttackUpdate();
		break;
	case ActionBehavior::kFireBall:
		//ファイアーボール
		FireBallUpdate();
		break;
	}

}

void PlayerCommandAcition::ActionInit(){
	flameTime_ = 30.0f;
	animationNumber_ = 0;

	isAttack_ = false;
	actionEnd_ = false;
}
void PlayerCommandAcition::SetRequest(AttackType type){

	//アクションのリクエスト
	if (type == AttackType::kNormalAttack) {
		behaviorRequest_ = ActionBehavior::kNomalAttack;
	}else if(type == AttackType::kFireBall){
		behaviorRequest_ = ActionBehavior::kFireBall;
	}

}

void PlayerCommandAcition::NomalAttaclInit(){
	animationNumber_ = PlayerAnimationNumber::kAnimeAttack;

	animation_->SetpreAnimationTimer(0);
}
void PlayerCommandAcition::NomalAttackUpdate(){
	if (animation_->GetAnimationTimer() > CommandActionConstants::kAttackAnimationStartTime) {
		flameTime_ = 15.0f;
		isAttack_ = true;
	}

	if (animation_->GetAnimationTimer() >= CommandActionConstants::kAttackAnimationEndTime) {
		actionEnd_ = true;
	}
}

void PlayerCommandAcition::FireBallInit(){
	animation_->SetpreAnimationTimer(0);
}
void PlayerCommandAcition::FireBallUpdate(){

}
