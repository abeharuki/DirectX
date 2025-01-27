#include "TransitionEffect.h"


void TransitionEffect::Initialize(){

	//ゲームオーバー時のスプライト
	spriteClear_.reset(Sprite::CreateNoDepth("resources/clear.png"));
	spriteOver_.reset(Sprite::CreateNoDepth("resources/over.png"));
	spritePushA_.reset(Sprite::CreateNoDepth("resources/Title/push.png"));
	spriteClear_->SetSize({ 1280.0f, 720.0f });
	spriteOver_->SetSize({ 1280.0f, 720.0f });
	spritePushA_->SetSize({ 1280.0f,720.0f });
	spriteOver_->SetEdgeColor(TransitionEffectConstants::kDissolveEdgeColor);
	spritePushA_->SetEdgeColor(TransitionEffectConstants::kDissolveEdgeColor);
	spriteOver_->SetThreshold(spriteDissolve_);
	spritePushA_->SetThreshold(spriteDissolve_);

	//トランジション
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();
}

void TransitionEffect::Update(){

	//シーン遷移の更新
	transition_->Update();

	//クリア時
	GameClearEffect();

	//ゲームオーバー時
	GameOverEffect();

	//タイトルに戻る
	if (transition_->GetFade()) {
		SceneManager::GetInstance()->ChangeScene("TitleScene");
	}
}

void TransitionEffect::Draw(){
	//ゲームオーバーとクリアのスプライト
	spriteClear_->Draw();
	spriteOver_->Draw();
	spritePushA_->Draw();

	//シーン遷移の描画
	transition_->Draw();
}

void TransitionEffect::GameClearEffect(){
	//クリア時
	if (clear_) {

		if (clearSpritePos_.y < TransitionEffectConstants::kInitialClearSpriteY) {
			clearSpritePos_ = Math::Lerp(clearSpritePos_, { TransitionEffectConstants::kClearSpriteTargetX,0.0f,0.0f }, 0.2f);
		}
		else {
			transition_->ChangeScene();
			//フェードアウトし始めたら
			if (transition_->GetFadeOut()) {
				clearSpritePos_ = Math::Lerp(clearSpritePos_, { TransitionEffectConstants::kClearSpriteTargetX,TransitionEffectConstants::kClearSpriteFinalY,0.0f }, 0.1f);
			}

		}
		spritePushA_->SetThreshold(0.0f);
		spritePushA_->SetPosition({ clearSpritePos_.x,clearSpritePos_.y });
	}


	spriteClear_->SetPosition({ clearSpritePos_.x,clearSpritePos_.y });
}

void TransitionEffect::GameOverEffect(){
	//ゲームオバー時
	if (over_) {
		spriteDissolve_ -= TransitionEffectConstants::kDissolveThresholdIncrement;
		//ディゾルブの値がゼロになったらフェイドの許可
		if (spriteDissolve_ <= 0) {
			transition_->ChangeScene();
		}
		spriteOver_->SetThreshold(spriteDissolve_);
		spritePushA_->SetThreshold(spriteDissolve_);
	}
}
