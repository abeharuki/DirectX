#pragma once
#include "IScene.h"
#include <memory>
#include "Audio/Audio.h"

class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	Audio* audio_ = nullptr;
	uint32_t audioData_;
};