#pragma once
#include "IScene.h"
#include <memory>

class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	
};