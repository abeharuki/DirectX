#pragma once
#include "IScene.h"
#include <memory>
#include "Audio/Audio.h"
#include <Sprite.h>

class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	Audio* audio_ = nullptr;
	uint32_t audioData_;
	std::unique_ptr<Sprite> sprite_;
	Vector2 size;
};