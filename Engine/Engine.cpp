#include "Engine.h"

#include <Windows.h>
#include <cassert>

//	DirextXtex
#include <DirectXTex.h>

#include <d3dx12.h>
#include <vector>

#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include "Model.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "KeyInput.h"
#include <GlobalVariables.h>


WinApp* win = nullptr;
DirectXCommon* dxCommon = nullptr;
GameScene* gameScene = nullptr;
ImGuiManager* imguiManager = nullptr;
KeyInput* keyInput = nullptr;

Engine* Engine::GetInstance() {
	static Engine instance;
	return &instance;
}

void Engine::Initialize(const char* title, int width, int height) {
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win, 1280, 720);

	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	 // ゲームシーンの初期化
	gameScene = GameScene::GetInstance();
	gameScene->Initialize();
	
	//	Inputの初期化処理
	keyInput = KeyInput::GetInstance();
	keyInput->Initialize();
	//KeyInput::InputInitialize();
}

bool Engine::BeginFrame() { 
	
	if (!win->ProcessMessage()) {
		return false;
	}


	return true;
}

void Engine::Finalize() {

	dxCommon->Debug();
	//  DirectX終了処理
	dxCommon->Finalize();
	dxCommon = nullptr;
	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
	win = nullptr;
}

void Engine::EndFrame() {
	// ImGui受付開始
	imguiManager->Begin();
	// グローバル変数の更新
	GlobalVariables::GetInstance()->Updeat();

	//	Input初期の更新
	keyInput->Update();

	gameScene->Update();

	// 描画開始
	dxCommon->PreDraw();
	
	gameScene->Draw();
#ifdef _DEBUG
	
	// ImGui受付終了
	imguiManager->End();
	// ImGui描画
	imguiManager->Draw();
#endif 
	// 描画終了
	dxCommon->PostDraw();
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::GetCPUDescriptorHandle(
    ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE Engine::GetGPUDescriptorHandle(
    ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}




Microsoft::WRL::ComPtr<ID3D12Device> Engine::GetDevice() { return dxCommon->GetDevice(); }

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Engine::GetList() {
	return dxCommon->GetCommandList();
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Engine::GetSRV() { return dxCommon->GetSRV(); }

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Engine::GetRTV() { return dxCommon->GetRTV(); }

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Engine::GetDSV() { return dxCommon->GetDSV(); }

