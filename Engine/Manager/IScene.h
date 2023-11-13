#pragma once
#include "KeyInput.h"

// シーン追加
enum SCENE {
	TITLE,
	GAME,
	CLEAR,
	SCENEMAX,
};

class IScene {

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual ~IScene();

	static int sceneNo_;
};