#include "Game.h"

void Game::Initialize() {
	GameManager::Initialize();

	// シーンファクトを生成し、マネージャーにセット
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);
	// シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("TitleScene");
}

void Game::Update() {
	// 基底クラスの更新
	GameManager::Update();
}

void Game::Draw() {
	// 基底クラスの描画
	GameManager::Draw();
}