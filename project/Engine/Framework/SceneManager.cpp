#include "SceneManager.h"
#include <cassert>


SceneManager* SceneManager::instance_ = nullptr;

SceneManager* SceneManager::GetInstance() {
	
	if (instance_ == nullptr) {
		instance_ = new SceneManager();
	}
	return instance_;
}

void SceneManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void SceneManager::Update() {
	
	
  	if (load_) {
		loadScene_->Update();
	}
	else {
		currentScene_->Update();
	}
}

void SceneManager::Draw() { 
	if (!load_) {
		currentScene_->Draw();
	}
	else {
		loadScene_->Draw();
	}
	
}

void SceneManager::RenderDirect() {
	if (!load_) {
		currentScene_->RenderDirect();
	}
	else {
		loadScene_->RenderDirect();
	}
	
}

void SceneManager::LoadScene() {
	loadScene_ = sceneFactory_->CreateScene("LoadScene");
	loadScene_->Initialize();
}

void SceneManager::Loading() { 
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
		load_ = false;
		loadScene_->Initialize();
	}


}

void SceneManager::FirstScene(const std::string& sceneName) {
	currentScene_ = sceneFactory_->CreateScene(sceneName);
	currentScene_->Initialize();
}

void SceneManager::ChangeScene(const std::string& sceneName) {

	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	nextScene_ = sceneFactory_->CreateScene(sceneName);
	load_ = true;
}



SceneManager::~SceneManager() {
	delete currentScene_;
	delete loadScene_;
}
