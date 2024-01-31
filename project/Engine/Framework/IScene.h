#pragma once

class SceneManager;

class IScene {

public:
	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual ~IScene() = default;

	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; };


protected:
	SceneManager* sceneManager_ = nullptr;
	
};