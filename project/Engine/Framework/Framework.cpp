#include "Framework.h"

void Framework::Initialize() { sceneManager_ = SceneManager::GetInstance(); }

void Framework::Update() { sceneManager_->Update(); }

void Framework::Draw() { sceneManager_->Draw(); }

void Framework::Finalize() { delete sceneFactory_; }

void Framework::Run() {
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;
	Engine::Initialize(L"自作エンジン", windowWidth, windowHeight);
	Initialize();

	// ウインドウの×ボタンが押されるまでループ
	while (true) {

		// メッセージ処理
		if (Engine::ProcessMessage()) {
			break;
		}

		Engine::BeginFrame();
		
		Update();

		Draw();

		Engine::EndFrame();
	}

	Finalize();

	Engine::Finalize();


}