#pragma once
#include "Framework/GameManager.h"
#include "SceneFactory.h"

class Game : public GameManager {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private:
};