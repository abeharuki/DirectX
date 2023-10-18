#include "Engine.h"

#include <Windows.h>
#include <cassert>

//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"

#include "externals/DirectXTex/d3dx12.h"
#include <vector>

#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "Model.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "KeyInput.h"

WinApp* win = nullptr;
DirectXCommon* dxCommon = nullptr;
Model* model = nullptr;
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
	//  DirectX終了処理
	dxCommon->Finalize();
	// ゲームウィンドウの破棄
	win->TerminateGameWindow();
}

void Engine::EndFrame() {
	// ImGui受付開始
	imguiManager->Begin();

	//	Input初期の更新
	//KeyInput::Update();
	keyInput->Update();

	gameScene->Update();

	// ImGui受付終了
	imguiManager->End();

	// 描画開始
	dxCommon->PreDraw();
	
	gameScene->Draw();

	// ImGui描画
	imguiManager->Draw();
	// 描画終了
	dxCommon->PostDraw();
}


Microsoft::WRL::ComPtr<ID3D12Device> Engine::GetDevice() { return dxCommon->GetDevice(); }

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Engine::GetList() {
	return dxCommon->GetCommandList();
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Engine::GetSRV() { return dxCommon->GetSRV(); }

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Engine::GetRTV() { return dxCommon->GetRTV(); }

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Engine::GetDSV() { return dxCommon->GetDSV(); }

