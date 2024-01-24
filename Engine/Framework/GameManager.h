#pragma once
#include <Engine.h>
#include "SceneManager.h"


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