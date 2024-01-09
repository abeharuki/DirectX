#include "ClearScene.h"

void ClearScene::Initialize() { 
	spriteClear_.reset(Sprite::Create("resources/clear.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));
}

void ClearScene::Update() {
	spriteClear_->SetSize({3.6f, 2.0f});
	spritePushA_->SetSize({3.6f, 2.0f});

	if (KeyInput::GetInstance()->GetPadConnect()) {
		if (KeyInput::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			sceneNo_ = GAME;
		}
	}

	if (KeyInput::PushKey(DIK_P)) {
		sceneNo_ = TITLE;
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