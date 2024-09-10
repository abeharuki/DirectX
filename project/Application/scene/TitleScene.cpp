#include "TitleScene.h"
#include "Framework/SceneManager.h"



void TitleScene::Initialize() {
	
	

	

	audio_ = Audio::GetInstance();
	audioData_[0] = audio_->SoundLoadMP3("resources/audio/BGM.mp3");
	audio_->SoundPlayMP3(audioData_[1], true, 3.0f);

	
	alpha_ = 1.0f;
	// フェードイン・フェードアウト用スプライト
	spriteBack_.reset(Sprite::Create("resources/Black.png"));
	rule_ = false;
	
	spriteBack_->SetSize({ 1280.0f,720.0f });
	isFadeIn_ = true;
	isFadeOut_ = false;
	isFede_ = false;


	PostEffect::GetInstance()->isGrayscale(false);
	PostEffect::GetInstance()->isOutLine(true);
	//PostEffect::GetInstance()->isBloom(true);
	
}

void TitleScene::Update() {
	spriteBack_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });
	if (Input::GetInstance()->GetPadConnect()) {
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			//rule_ = true;
		}
	}

	if (Input::PushKey(DIK_G)) {
		//rule_ = true;
	}
	

	if (Input::GetInstance()->GetPadConnect()) {
		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A) && !isFadeIn_) {
			isFadeOut_ = true;
		}

		if (Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B) && !isFadeIn_) {
			//rule_ = true;
			//isFadeOut_ = true;
		}

	}

	if (Input::PushKey(DIK_G) && !isFadeIn_) {
		isFadeOut_ = true;
	}

	if (Input::PushKey(DIK_H) && !isFadeIn_) {
		//rule_ = true;
		//isFadeOut_ = true;
	}




	Fade();
	
	

	if (Input::PushKey(DIK_C)) {
		SceneManager::GetInstance()->ChangeScene("ClearScene");
	}

	if (Input::PushKey(DIK_O)) {
		SceneManager::GetInstance()->ChangeScene("OverScene");
	}
	PostEffect::GetInstance()->ValueOutLine(a_);

	ImGui::Begin("Player");
	ImGui::End();

	ImGui::Begin("Setting");
	ImGui::DragFloat3("DirectionLight", &directionLight_.direction.x, 1.0f);
	ImGui::DragFloat("Light", &directionLight_.intensity, 1.0f);
	ImGui::DragFloat("outline", &a_, 0.1f);
	ImGui::DragFloat2("TitlePos", &pos_.x, 1.0f);
	ImGui::End();
}

void TitleScene::Draw() {
	// 3Dオブジェクト描画前処理
	
}

void TitleScene::RenderDirect() {
	
	spriteBack_->Draw();
}

void TitleScene::cameraMove() {

	

}


void TitleScene::Fade() {
	if (isFadeIn_) {
		if (alpha_ > 0.0f) {
			alpha_ -= 0.02f;
		}
		else {
			alpha_ = 0.0f;
			isFadeIn_ = false;
		}
	}

	if (isFadeOut_) {
		if (alpha_ < 1) {
			alpha_ += 0.02f;
		}
		else {
			alpha_ = 1.0f;
			if (rule_) {
				//SceneManager::GetInstance()->ChangeScene("TutorialScene");
			}
			else {
				SceneManager::GetInstance()->ChangeScene("GameScene");
			}
			
		}
	}
}