#pragma once
#include "Framework/IScene.h"
#include "KeyInput.h"
#include <Sprite.h>

class OverScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	std::unique_ptr<Sprite> spriteOver_;
	std::unique_ptr<Sprite> spritePushA_;
};