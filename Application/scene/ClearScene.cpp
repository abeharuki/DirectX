#include "ClearScene.h"
#include "Framework/SceneManager.h"

void ClearScene::Initialize() { 
	spriteClear_.reset(Sprite::Create("resources/clear.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));
}

void ClearScene::Update() {
	spriteClear_->SetSize({3.6f, 2.0f});
	spritePushA_->SetSize({3.6f, 2.0f});

	if (KeyInput::GetInstance()->GetPadConnect()) {
		if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			
		}
	}

	if (KeyInput::PushKey(DIK_P)) {
		sceneManager_->ChangeScene("TitleScene");
	}
}

void ClearScene::Draw() {
	Transform uv;
	uv.scale = {0.0f, 0.0f, 0.0f};
	uv.rotate = {0.0f, 0.0f, 0.0f};
	uv.translate = {0.0f, 0.0f, 0.0f};
	spriteClear_->Draw(uv);
	spritePushA_->Draw(uv);
}