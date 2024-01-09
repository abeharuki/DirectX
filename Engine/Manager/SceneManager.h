#pragma once
#include "OverScene.h"
#include "ClearScene.h"
#include "GameScene.h"
#include "IScene.h"
#include "TitleScene.h"
#include <memory>

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	void Finalize();
	int Run();

private:
	// シーンを保持するメンバ変数
	std::unique_ptr<IScene> sceneArr_[SCENEMAX];

	int currentsceneNo_; // 現在のシーン
	int prevSceneNo_;    // 前のシーン
};