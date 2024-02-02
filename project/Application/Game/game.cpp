#include "Game.h"

void Game::Initialize() { 
	Framework::Initialize();


	//シーンファクトを生成し、マネージャーにセット
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);
	//シーンマネージャーに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("TitleScene");

}

void Game::Update() {
	// 基底クラスの更新
	Framework::Update();
}

void Game::Draw() {
	// 基底クラスの描画
	Framework::Draw();
}