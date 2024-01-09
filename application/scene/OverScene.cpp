#include "OverScene.h"

void OverScene::Initialize() { 
	spriteOver_.reset(Sprite::Create("resources/over.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));

}

void OverScene::Update() {
	spriteOver_->SetSize({3.6f, 2.0f});
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

void OverScene::Draw() {
	Transform uv;
	uv.scale = {0.0f, 0.0f, 0.0f};
	uv.rotate = {0.0f, 0.0f, 0.0f};
	uv.translate = {0.0f, 0.0f, 0.0f};
	spriteOver_->Draw(uv);
	spritePushA_->Draw(uv);
}