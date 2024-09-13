#include "TitleScene.h"
#include "Framework/SceneManager.h"
#include <Player/DebugPlayer.h>
#include <numbers>



void TitleScene::Initialize() {

	Load();
	Save();

	sceneManager_ = SceneManager::GetInstance();

	pressStart_ = AudioHelper{ "resources/audio/jump.mp3" };
	bgm_ = AudioHelper{ "resources/audio/bgm.mp3" };
	bgm_.Play(true, 0.8f);

	alpha_ = 1.0f;
	// フェードイン・フェードアウト用スプライト
	spriteBack_ = std::unique_ptr<Sprite>(Sprite::Create("resources/Black.png"));
	rule_ = false;

	spriteBack_->SetSize({ 1280.0f,720.0f });
	isFadeIn_ = true;
	isFadeOut_ = false;
	isFede_ = false;

	// タイトル文字の設定
	{
		titleText_ = std::unique_ptr<Sprite>(Sprite::Create("resources/Title/title.png"));

		titleText_->SetAnchorPoint({ 0.5f,0.5f });
		titleText_->SetSize(Vector2{ 768.f,384.f }*vCenerOffset_.second.z);
		titleText_->SetPosition(Vector2{ 1024.f, 704.f } *0.5f + vCenerOffset_.second.GetVec2());
	}
	{
		background_ = std::unique_ptr<Sprite>(Sprite::Create("resources/Stage/background_0.png"));

		background_->SetAnchorPoint({ 0.5f,0.5f });
		background_->SetSize(Vector2{ 1024.f, 704.f });
		background_->SetPosition(Vector2{ 1024.f, 704.f }*0.5f);
	}
	{
		startText_ = std::unique_ptr<Sprite>(Sprite::Create("resources/Title/text1.png"));

		startText_->SetAnchorPoint({ 0.5f,0.5f });
		startText_->SetPosition(Vector2{ 1024.f, 704.f } *0.5f + vButtonOffset_.second.GetVec2());
		startText_->SetSize(Vector2{ 268.f, 37.f }*(vButtonOffset_.second.z));
	}

	auto *postEffect = PostEffect::GetInstance();
	postEffect->isGrayscale(false);
	postEffect->isOutLine(true);
	animFlame_ = 0;

}

void TitleScene::Update() {
	Load();
	const auto *const input = Input::GetInstance();
	// フェード用のスプライト
	spriteBack_->SetColor({ 1.0f, 1.0f, 1.0f, alpha_ });

	if (input->GetPadConnect()) {
		if (input->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			StartTransition();
		}
	}

	if (Input::PushKey(DIK_SPACE)) {
		StartTransition();
	}

	Fade();

#ifdef _DEBUG

	if (Input::PushKey(DIK_C)) {
		sceneManager_->ChangeScene("ClearScene");
	}

	if (Input::PushKey(DIK_O)) {
		sceneManager_->ChangeScene("OverScene");
	}

#endif // _DEBUG

	// タイトルのアニメーション
	{
		const float t = std::sin(static_cast<float>((animFlame_ % vMoveTime_.second)) / vMoveTime_.second * std::numbers::pi_v<float> *2);

		titleText_->SetPosition(Vector2{ 1024.f, 704.f } *0.5f + vCenerOffset_.second.GetVec2() + vMoveSize_.second.GetVec2() * t);
		titleText_->SetSize(Vector2{ 768.f,384.f }*(vCenerOffset_.second.z + vMoveSize_.second.z * t));

		animFlame_++;
	}

#ifdef _DEBUG
	
	// UIのアニメーション
	{

		startText_->SetPosition(Vector2{ 1024.f, 704.f } *0.5f + vButtonOffset_.second.GetVec2());
		startText_->SetSize(Vector2{ 268.f, 37.f }*(vButtonOffset_.second.z));

		animFlame_++;
	}

#endif // _DEBUG


	titleText_->UpdateVertexBuffer();
	background_->UpdateVertexBuffer();
	startText_->UpdateVertexBuffer();

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

	background_->Draw();
	startText_->Draw();
	titleText_->Draw();
}

void TitleScene::RenderDirect() {

	spriteBack_->Draw();
}

void TitleScene::cameraMove() {



}

void TitleScene::StartTransition()
{
	if (not isFadeIn_) {
		isFadeOut_ = true;

		pressStart_.Play(false, 1.f);
	}
}

void TitleScene::Save() const
{
	JsonLoadHelper helper{ "Title" };
	helper << vCenerOffset_;
	helper << vMoveSize_;
	helper << vMoveTime_;
	helper << vButtonOffset_;
}

void TitleScene::Load()
{
	const JsonLoadHelper helper{ "Title" };
	helper >> vCenerOffset_;
	helper >> vMoveSize_;
	helper >> vMoveTime_;
	helper >> vButtonOffset_;
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
				sceneManager_->ChangeScene("GameScene");
			}

		}
	}
}

TitleScene::~TitleScene()
{
	bgm_.Stop();
}
