#pragma once
#include "ClearScene.h"
#include "Framework/AbstractSceneFactory.h"
#include "GameScene.h"
#include "OverScene.h"
#include "TitleScene.h"

class SceneFactory : public AbstractSceneFactory {
public:
	IScene* CreateScene(const std::string& sceneName) override;
};