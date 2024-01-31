#pragma once
#include "Framework/IScene.h"
#include "KeyInput.h"

class ClearScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	
};