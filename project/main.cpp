#include "Application/Game/game.h"

// WIndowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	std::unique_ptr<Framework> game = std::make_unique<Game>();
	game->Run();
	return 0;
}



