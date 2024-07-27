#pragma once
#include "IScene.h"
#include "AbstractSceneFactory.h"

class SceneManager {
public:
	static SceneManager* GetInstance();
	static void Destroy();

	~SceneManager();

	void Update();

	void Draw();	

	void  RenderDirect();

	void Loading();

	void LoadScene();

	void FirstScene(const std::string& sceneName);
	
	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

	bool ChangeScene() { return load_; }

private:
	static SceneManager* instance_;

	IScene* currentScene_ = nullptr; // 現在のシーン
	IScene* nextScene_ = nullptr;    // 次のシーン

	IScene* loadScene_ = nullptr;//ロード画面
	bool load_ = false;

	AbstractSceneFactory* sceneFactory_ = nullptr;


	int currentItem = 0; // 初期選択アイテムのインデックス
	int preCurrentItem = 0;
};