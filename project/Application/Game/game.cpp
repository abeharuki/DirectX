#include "Game.h"

void Game::Initialize() { 
	Framework::Initialize();


	//シーンファクトを生成し、マネージャーにセット
	sceneFactory_ = new SceneFactory();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_);
	//ロード画面の設定
	SceneManager::GetInstance()->LoadScene();
	//シーンマネージャーに最初のシーンをセット
	//SceneManager::GetInstance()->FirstScene("TitleScene");
	//SceneManager::GetInstance()->FirstScene("GameScene");
	SceneManager::GetInstance()->FirstScene("DebugScene");
}

void Game::Update() {



	// 基底クラスの更新
	Framework::Update();

	
}

void Game::Draw() {
	// 基底クラスの描画
	Framework::Draw();
}

