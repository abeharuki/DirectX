#include "ImGuiManager.h"

#ifdef _DEBUG
#include "DirectXCommon.h"
#include "WinApp.h"

#endif




ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(
    WinApp* winApp,DirectXCommon* dxCommon) {
#ifdef _DEBUG
	

	dxCommon_ = dxCommon;

	

	// ImGuiの初期化。
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
	    dxCommon_->GetDevice(), static_cast<int> (dxCommon_->GetBackBufferCount()),
	    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, dxCommon_->GetSRV(),
	    dxCommon_->GetSRV()->GetCPUDescriptorHandleForHeapStart(),
	    dxCommon_->GetSRV()->GetGPUDescriptorHandleForHeapStart());
#endif
}


void ImGuiManager::Begin() {
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	
#endif
}

void ImGuiManager::End() {
#ifdef _DEBUG

	// ImGuiの内部コマンドを生成する
	ImGui::Render();
#endif
}


void ImGuiManager::Draw() {
#ifdef _DEBUG
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	
	// デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = {dxCommon_->GetSRV()};
	commandList->SetDescriptorHeaps(1, ppHeaps);
	// 画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	// 実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

#endif
}
