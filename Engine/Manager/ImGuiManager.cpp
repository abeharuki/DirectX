#include "ImGuiManager.h"


#include "DirectXCommon.h"
#include "WinApp.h"





ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(
    WinApp* winApp,DirectXCommon* dxCommon) {

	

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

}


void ImGuiManager::Begin() {

	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	

}

void ImGuiManager::End() {


	// ImGuiの内部コマンドを生成する
	ImGui::Render();

}


void ImGuiManager::Draw() {

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	
	// デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = {dxCommon_->GetSRV()};
	commandList->SetDescriptorHeaps(1, ppHeaps);
	// 画面に描く処理はすべて終わり、画面に映すので、状態を遷移
	// 実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);


}
