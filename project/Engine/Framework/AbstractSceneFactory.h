#pragma once
#include "IScene.h"
#include <string>

class AbstractSceneFactory {
public:
	// 仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;
	// シーン遷移
	virtual IScene* CreateScene(const std::string& sceneName) = 0;
};
