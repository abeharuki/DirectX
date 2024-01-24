#pragma once
#include "AbstractSceneFactory.h"
#include "IScene.h"

class SceneManager {
public:
	static SceneManager* GetInstance();

	~SceneManager();

	void Update();

	void Draw();

	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

private:
	static SceneManager* instance_;

	IScene* currentScene_ = nullptr; // 現在のシーン
	IScene* nextScene_ = nullptr;    // 次のシーン

	AbstractSceneFactory* sceneFactory_ = nullptr;
};
