#pragma once
#include <Engine.h>
#include "SceneManager.h"
#include "PostEffects/PostEffect.h"

class Framework {
public:
	
	virtual void Initialize();

	virtual void Update();

	virtual void Draw();


	void Finalize();

	void Run();

protected:
	SceneManager* sceneManager_ = nullptr;
	AbstractSceneFactory* sceneFactory_ = nullptr;
	PostEffect* postEffect_ = nullptr;
	bool nowLoading = false;
};