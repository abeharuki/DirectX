#include "SceneFactory.h"

IScene* SceneFactory::CreateScene(const std::string& sceneName) {

	//次のシーンを作成
	IScene* newScene = nullptr;

	if (sceneName == "TitleScene") {
		newScene = new TitleScene();
	} else if (sceneName == "GameScene") {
		newScene = new GameScene();
	} else if (sceneName == "ClearScene") {
		newScene = new ClearScene();
	} else if (sceneName == "OverScene") {
		newScene = new OverScene();
	}
	else if (sceneName == "LoadScene") {
		newScene = new LoadScene();
	}


	return newScene;
}