#pragma once
#include "Framework/Framework.h"
#include "SceneFactory.h"
#include "GlobalVariables.h"

class Game : public Framework {
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



	GlobalVariables *pGVariables_ = nullptr;

};