#include "Application/Game/game.h"

// WIndowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	GameManager* game = new Game();
	game->Run();
	delete game;
	return 0;
}



