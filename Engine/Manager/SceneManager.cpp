#include "SceneManager.h"
#include "ImGuiManager.h"
#include "Engine.h"

SceneManager::SceneManager() {
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[GAME] = std::make_unique<GameScene>();
	sceneArr_[CLEAR] = std::make_unique<ClearScene>();
	sceneArr_[OVER] = std::make_unique<OverScene>();

	IScene::sceneNo_ = TITLE;
}

SceneManager::~SceneManager() {}

int SceneManager::Run() {
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;
	Engine::Initialize(L"自作エンジン", windowWidth, windowHeight);
	
	// ウインドウの×ボタンが押されるまでループ
	while (true) {

		// メッセージ処理
		if (Engine::ProcessMessage()) {
			break;
		}

		Engine::BeginFrame();
		prevSceneNo_ = currentsceneNo_;

		currentsceneNo_ = IScene::sceneNo_;

		

		if (prevSceneNo_ != currentsceneNo_) {
			
			sceneArr_[currentsceneNo_]->Initialize();
		}

		

		// 更新処理
		sceneArr_[currentsceneNo_]->Update(); // シーンごとの更新


		// 描画処理
		sceneArr_[currentsceneNo_]->Draw();

		Engine::EndFrame();
	}

	Engine::Finalize();

	return 0;
}