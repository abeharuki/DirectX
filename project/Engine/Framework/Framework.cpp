#include "Framework.h"
#include <queue>

void Framework::Initialize() { sceneManager_ = SceneManager::GetInstance(); }

void Framework::Update() { 
	Engine::BeginFrame();
	sceneManager_->Update(); 
	//sceneManager_->LoadingScreenUpdate();
	Engine::EndFrame();
}

void Framework::Draw() { 
	Engine::PreDraw();
	if (nowLoading) {
		//sceneManager_->LoadingScreenDraw();
		
	}
	if(loadingFinished) {
		
	}
	sceneManager_->Draw();
	Engine::PostDraw();
}

void Framework::LoadingScreenUpdate() { 
	Engine::BeginFrame();
	
	Engine::EndFrame();
}

void Framework::LoadingScreenDraw() { 
	Engine::PreDraw();
	
	Engine::PostDraw();
	}

void Framework::Finalize() { delete sceneFactory_; }

void Framework::Run() {
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;
	std::mutex mutex;
	std::condition_variable condition;
	std::queue<int>q;
	Engine::Initialize(L"自作エンジン", windowWidth, windowHeight);
	Initialize();

	

	//バックグラウンドループ
	std::thread backgroundThread([&]() {
		{
			//std::unique_lock<std::mutex>uniqueLock(mutex);
			//condition.wait(uniqueLock, [&]() {return !q.empty(); });
			//q.pop();
			std::unique_lock<std::mutex> uniqueLock(mutex);
			condition.wait(uniqueLock, [&]() { return nowLoading; });
			if (!sceneManager_->ChangeScene()) {
				loadingFinished = true;
			}


		}
		while (true) {
			//std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			
			
			
		}

		
	});



	// ウインドウの×ボタンが押されるまでループ
	while (true) {

	
		// メッセージ処理
		if (Engine::ProcessMessage()) {
			break;
		}

		
		
		//LoadingScreenUpdate();
		Update();
		if (nowLoading) {
			//LoadingScreenDraw();
		}	
	
		Draw();
		
		

		if (sceneManager_->ChangeScene()) {

			{
				std::lock_guard<std::mutex> lock(mutex);
				loadingFinished = false; // ロードフラグをリセット	
				nowLoading = true;
				condition.notify_all();
			}
			
		}
		
	}

	

	// バックグラウンドスレッドの終了
	backgroundThread.join();

	Finalize();
	Engine::Finalize();


}