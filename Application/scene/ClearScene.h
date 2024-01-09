#pragma once
#include "IScene.h"
#include <Sprite.h>


class ClearScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	std::unique_ptr<Sprite> spriteClear_;
	std::unique_ptr<Sprite> spritePushA_;
};