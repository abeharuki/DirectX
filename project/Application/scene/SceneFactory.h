#pragma once
#include "Framework/AbstractSceneFactory.h"
#include "TutorialScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "ClearScene.h"
#include "OverScene.h"
#include "LoadScene.h"
#include "DebugScene.h"

class SceneFactory : public AbstractSceneFactory {
public:
	IScene* CreateScene(const std::string& sceneName) override;


};