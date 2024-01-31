#include "GameManager.h"

void GameManager::Initialize() { sceneManager_ = SceneManager::GetInstance(); }

void GameManager::Update() { sceneManager_->Update(); }

void GameManager::Draw() { sceneManager_->Draw(); }

void GameManager::Finalize() { delete sceneFactory_; }

void GameManager::Run() {
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