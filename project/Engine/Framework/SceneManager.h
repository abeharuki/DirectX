#pragma once
#include "IScene.h"
#include "AbstractSceneFactory.h"
#include <imgui.h>

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

	//デバック用
	void DebugSceneChange() {
		const char* items[] = { "TitleScene", "GameScene", "ClearScene", "OverScene", "DebugScene" };
		if (!load_) {
			if (currentItem != preCurrentItem) {
				preCurrentItem = currentItem;
				ChangeScene(items[currentItem]);  // 配列からシーン名を取得
			}

			ImGui::Begin("Scene");
			ImGui::Combo("##combo", &currentItem, items, IM_ARRAYSIZE(items));
			ImGui::End();
		}
		
	}

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