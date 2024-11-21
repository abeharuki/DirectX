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
#include "ImGuiManager.h"
#include "Input.h"
#include <GlobalVariables.h>
#include "Audio/Audio.h"
#include "Manager/ParticleManager.h"

WinApp* win = nullptr;
DirectXCommon* dxCommon = nullptr;
ImGuiManager* imguiManager = nullptr;
Input* keyInput = nullptr;
Audio* audio = nullptr;
ParticleManager* particleManager = nullptr;
float Engine::gameTime = 0;

Engine* Engine::GetInstance() {
	static Engine instance;
	return &instance;
}

void Engine::Initialize(const wchar_t* title, int width, int height) {
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(title, width, height);

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win, 1280, 720);

	imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	TextureManager::GetInstance()->Initialize();
	TextureManager::GetInstance()->Load("resources/white.png");

	//	Inputの初期化処理
	keyInput = Input::GetInstance();
	keyInput->Initialize();

	audio = Audio::GetInstance();
	audio->Initialize();
}

bool Engine::ProcessMessage() {
	if (!win->ProcessMessage()) {
		return false;
	}

	return true;
}


void Engine::BeginFrame() {
	// ImGui受付開始
	imguiManager->Begin();
	// グローバル変数の更新
	GlobalVariables::GetInstance()->Updeat();

	//	Input初期の更新
	keyInput->Update();

	gameTime += 1.0f/60.0f;

}

void Engine::Finalize() {

	audio->Finalize();

	//  DirectX終了処理
	dxCommon->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	//dxCommon->Debug();

	TextureManager::GetInstance()->Destroy();

	ParticleManager::GetInstance()->Destroy();
}

void Engine::EndFrame() {
	// ImGui受付終了
	imguiManager->End();

}

void Engine::RenderPreDraw() {
	dxCommon->RenderPreDraw();
}

void Engine::RenderPostDraw() {
	dxCommon->RenderPostDraw();
}

void Engine::DepthPreDraw() {
	dxCommon->DepthPreDraw();
}

void Engine::DepthPostDraw() {
	dxCommon->DepthPostDraw();
}

void Engine::UAVBarrier(GpuResource& resource){
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = resource.GetResource();
	dxCommon->GetCommandList()->ResourceBarrier(1, &barrier);
}



void Engine::PreDraw() {
	// 描画開始
	dxCommon->PreDraw();
}

void Engine::PostDraw() {
#ifdef _DEBUG
	// ImGui描画
	imguiManager->Draw();
#endif 
	// 描画終了
	dxCommon->PostDraw();
}



D3D12_CPU_DESCRIPTOR_HANDLE Engine::GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
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

const DescriptorHandle& Engine::GetHandle() { return dxCommon->GetHandle(); }
const DescriptorHandle& Engine::GetDepthHandle() { return dxCommon->GetDepthHandle(); }

//D3D12_GPU_DESCRIPTOR_HANDLE Engine::GetDSVHandle() { return dxCommon->GetDSVHandle(); }

void Engine::TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState)
{
	D3D12_RESOURCE_STATES oldState = resource.GetResourceState();

	if (oldState != newState)
	{
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = resource.GetResource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = oldState;
		barrier.Transition.StateAfter = newState;
		resource.SetResourceState(newState);
		Engine::GetList()->ResourceBarrier(1, &barrier);
	}
}