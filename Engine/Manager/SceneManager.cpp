#include "SceneManager.h"
#include "ImGuiManager.h"
#include "Engine.h"

SceneManager::SceneManager() {
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[GAME] = std::make_unique<GameScene>();
	sceneArr_[CLEAR] = std::make_unique<ClearScene>();

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

		ImGui::Begin("Scene");

		if (currentsceneNo_ == 0) {
			ImGui::Text("Scene::Title");
			ImGui::Text("State PushKey(P)");
		}

		if (currentsceneNo_ == 1) {
			ImGui::Text("Scene::Game");
		}

		if (currentsceneNo_ == 2) {
			ImGui::Text("Scene::Clear");
			ImGui::Text("Title PushKey(P)");
		}

		ImGui::End();

		// 描画処理
		sceneArr_[currentsceneNo_]->Draw();

		Engine::EndFrame();
	}

	Engine::Finalize();

	return 0;
}