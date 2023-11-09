#include <Windows.h>
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <wrl.h>
#include "Math.h"

#include "WinApp.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "Model.h"
#include "ImGuiManager.h"
#include "Engine.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


#pragma comment(lib, "dxcompiler.lib")







// WIndowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;
	Engine::Initialize(L"自作エンジン", windowWidth, windowHeight);


	// ウインドウの×ボタンが押されるまでループ
	while (true) {
		

		// メッセージ処理
		if (Engine::BeginFrame()) {break;}

		
		Engine::EndFrame();

		
	}
	
	
	Engine::Finalize();
	

	return 0;
}



