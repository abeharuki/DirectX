#include "Framework.h"
#include <queue>
#include <condition_variable>

void Framework::Initialize() { sceneManager_ = SceneManager::GetInstance(); }

void Framework::Update() { 
	Engine::BeginFrame();
	if (sceneManager_->ChangeScene()) {
		nowLoading = true;
	}
	sceneManager_->Update(); 
	
	Engine::EndFrame();
}

void Framework::Draw() { 
	Engine::PreDraw();
	sceneManager_->Draw();
	Engine::PostDraw();
}


void Framework::Finalize() { 
	Engine::Finalize();
	delete sceneFactory_; 
}

void Framework::Run() {
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;
	std::mutex mutex;
	std::condition_variable condition;
	bool exit = false;
	Engine::Initialize(L"自作エンジン", windowWidth, windowHeight);
	Initialize();

	

	//バックグラウンドループ
	std::thread backgroundThread([&]() {
		
		while (!exit) {
		
			{
				
				std::unique_lock<std::mutex> uniqueLock(mutex);
				condition.wait(uniqueLock, [&]() { return true; });
				if (nowLoading) {
					sceneManager_->Loading();
					nowLoading = false;
				}


			}
			
			
		}

		
	});



	// ウインドウの×ボタンが押されるまでループ
	while (true) {

	
		// メッセージ処理
		if (Engine::ProcessMessage()) {
			break;
		}

		Update();
		
		Draw();		
		
	}

	
	exit = true;
	// バックグラウンドスレッドの終了
	backgroundThread.join();

	Finalize();
	


}