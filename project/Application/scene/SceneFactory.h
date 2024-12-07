#pragma once
#include "Framework/AbstractSceneFactory.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "ClearScene.h"
#include "OverScene.h"
#include "LoadScene.h"
#include "DebugScene.h"

/**
 * @file SceneFactory
 * @brief シーン生成を管理するファクトリクラス 
 */
class SceneFactory : public AbstractSceneFactory {
public:
	IScene* CreateScene(const std::string& sceneName) override;


};