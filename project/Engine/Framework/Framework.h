#pragma once
#include <Engine.h>
#include "SceneManager.h"


class Framework {
public:
	
	virtual void Initialize();

	virtual void Update();

	virtual void Draw();

	virtual void LoadingScreenUpdate();
	virtual void LoadingScreenDraw();

	void Finalize();

	void Run();

protected:
	SceneManager* sceneManager_ = nullptr;
	AbstractSceneFactory* sceneFactory_ = nullptr;

	bool loadingFinished = false;
	bool nowLoading = false;
};