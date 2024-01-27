#pragma once
#include "Framework/IScene.h"
#include "KeyInput.h"
#include <memory>
#include "Audio/Audio.h"
#include <Sprite.h>

class TitleScene : public IScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void Fade();

private:
	Audio* audio_ = nullptr;
	uint32_t audioData_;
	std::unique_ptr<Sprite> spriteTitle_;
	std::unique_ptr<Sprite> spritePushA_;
	std::unique_ptr<Sprite> spriteRule_;

	// フェードイン・フェードアウト用スプライト
	std::unique_ptr<Sprite> spriteBack_;

	Vector2 pos_;
	bool rule_;

	bool isFadeOut_;
	bool isFadeIn_;
	bool isFede_;
	float alpha_;
};