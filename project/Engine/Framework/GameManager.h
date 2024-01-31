#pragma once
#include "SceneManager.h"
#include <Engine.h>

class GameManager {
public:
	virtual void Initialize();

	virtual void Update();

	virtual void Draw();

	void Finalize();

	void Run();

protected:
	SceneManager* sceneManager_ = nullptr;
	AbstractSceneFactory* sceneFactory_ = nullptr;
};
