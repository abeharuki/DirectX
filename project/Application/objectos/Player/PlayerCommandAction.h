#pragma once
#include <Animation/Animation.h>
#include <Command/Command.h>
#include <ParticleSystem.h>

namespace CommandActionConstants {
	// 攻撃関連
	const float kAttackAnimationStartTime = 0.8f;  // 攻撃アニメーションが開始される時間
	const float kAttackAnimationEndTime = 1.6f;    // 攻撃アニメーションが終了する時間
}

namespace PlayerAnimationNumber {
	enum Num {
		kAnimeAttack,//攻撃
		kDeath,//死亡
		kJump,//ジャンプ
		kRun,//移動
		kStandby,//待機
	};
	
};

/**
 * @file PlayerCommandAction
 * @brief プレイヤーの各コマンド時の動きを制御するクラス
 */
class PlayerCommandAcition {
public:
	// 振る舞い
	enum class ActionBehavior {
		kNomalAttack, // 通常攻撃
		kFireBall, // ファイアーボール
	};
	
	void Init(Animations* animation);//初期化
	void Update();//更新
	void ActionInit();//アクション初期化(変数やフラグの値をもとに戻す)

	//アクションが終わったかどうか
	bool GetActionEnd() { return actionEnd_; }
	bool IsAttack() { return isAttack_; }
	int GetAnimationNumber() { return animationNumber_; }
	float GetAnimationFlameTime() { return flameTime_; }

	//アクションのリクエスト
	void SetRequest(AttackType type);

private:
	//通常攻撃
	void NomalAttaclInit();
	void NomalAttackUpdate();

	//ファイアーボール
	void FireBallInit();
	void FireBallUpdate();

private:
	Animations* animation_;
	ActionBehavior behavior_ = ActionBehavior::kNomalAttack;
	// 次の振る舞いリクエスト
	std::optional<ActionBehavior> behaviorRequest_ = std::nullopt;

	float flameTime_;
	int animationNumber_;
	
	bool isAttack_;
	bool actionEnd_;
	

	//パーティクル
	ParticleSystem* particle_;
	EmitterSphere emitter_{};
};