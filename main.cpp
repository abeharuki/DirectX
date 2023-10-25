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
	Engine::Initialize("自作エンジン", windowWidth, windowHeight);

	
	

   
	// ウインドウの×ボタンが押されるまでループ
	while (true) {
		
		// メッセージ処理
		if (Engine::BeginFrame()) {break;}

		
		Engine::EndFrame();
		
	}
	
	/*
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	
	//解放処理
	ResourceObject deptStencilResource =
	    CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);
	CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
	
#ifdef _DEBUG
	debugController->Release();
#endif
	CloseWindow(hwnd);
	
	srvDescriptorHeap->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();
	materialResorce->Release();
	wvpResouce->Release();
	materialResourceSprite->Release();
	directionalLightResource->Release();
	transformationMatrixResourceSprite->Release();
	vertexResource->Release();
	vertexResourceSprite->Release();
	indexResourceSprite->Release();
	textureResource->Release();
	depthStencilResource->Release();
	dsvDescriptorHeap->Release();
	textureResource2->Release();
	indexResource->Release();

	
	}
	*/
	
	//リソースリークチェック
	//dxCommon->Debug();
	Engine::Finalize();
	

	return 0;
}



