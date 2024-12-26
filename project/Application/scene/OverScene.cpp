#include "OverScene.h"
#include "Framework/SceneManager.h"
#include <PostEffects/PostEffect.h>

void OverScene::Initialize() {
	spriteOver_.reset(Sprite::Create("resources/over.png"));
	spritePushA_.reset(Sprite::Create("resources/Title/push.png"));
	spriteOver_->SetSize({ 1280.0f, 720.0f });
	spritePushA_->SetSize({ 1280.0f,720.0f });

	transition_ = std::make_unique<Transition>();
	transition_->Initialize();

	//PostEffect::GetInstance()->isGrayscale(true);
}

void OverScene::Update() {
	transition_->ChangeScene();
	transition_->Update();
	
	if (transition_->GetFade()) {
		SceneManager::GetInstance()->ChangeScene("ClearScene");
	}
	spriteOver_->SetThreshold(dissolve_.threshold);
	spriteOver_->SetEdgeColor(dissolve_.edgeColor);
	spritePushA_->SetThreshold(dissolve_.threshold);
	spritePushA_->SetEdgeColor(dissolve_.edgeColor);

	ImGui::Begin("Sprite");
	ImGui::DragFloat("threshold", &dissolve_.threshold,0.01f,-1.0f,1.0f);
	ImGui::DragFloat3("edgeColor", &dissolve_.edgeColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::End();
}

void OverScene::Draw() {
	spriteOver_->Draw();
	spritePushA_->Draw();
	transition_->Draw();
}

void OverScene::RenderDirect() {

}

