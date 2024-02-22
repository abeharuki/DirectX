#include "SceneManager.h"
#include <cassert>


SceneManager* SceneManager::instance_ = nullptr;

SceneManager* SceneManager::GetInstance() {
	
	if (instance_ == nullptr) {
		instance_ = new SceneManager();
	}
	return instance_;
}

void SceneManager::Update() {

	//つぎのシーンの予約があるなら
	if (nextScene_) {
		//旧シーンの終了
		if (currentScene_) {
			delete currentScene_;
		}

		//シーン切り替え
		currentScene_ = nextScene_;
		nextScene_ = nullptr;

		//シーンマネージャーをセット
		currentScene_->SetSceneManager(this);

		//次のシーンを初期化
		currentScene_->Initialize();

	}
	
	


	currentScene_->Update();

}

void SceneManager::Draw() { 
	if (!load_) {
		currentScene_->Draw();
	}
	
}

void SceneManager::ChangeLoadScene(const std::string& sceneName) {
	loadScene_ = sceneFactory_->CreateScene(sceneName);
	loadScene_->Initialize();
}

void SceneManager::LoadingScreenUpdate() { 
	loadScene_->Update();
	
}

void SceneManager::LoadingScreenDraw() {
	loadScene_->Draw();
}

void SceneManager::ChangeScene(const std::string& sceneName) {

	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
	load_ = true;
}



SceneManager::~SceneManager() { delete currentScene_; }

