#include "GasianBlur.h"

void GasianBlur::Initialize() {
	sPipeline();
	CreateResource();

	colorBuffer_ = std::make_unique<ColorBuffer>();
	colorBuffer_->Create(WinApp::kWindowWidth, WinApp::kWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

}

void GasianBlur::Draw(DescriptorHandle srvHandle) {
	PreDraw();
	//ゼロなら止まる
	assert(blurData->stepWidth != 0);
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Engine::GetList()->SetGraphicsRootDescriptorTable(0, srvHandle);
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, resource_->GetGPUVirtualAddress());

	//  コマンドを積む
	//ビューポート
	D3D12_VIEWPORT viewport{ 0.0f, 0.0f, WinApp::kWindowWidth,WinApp::kWindowHeight, 0.0f, 1.0f };
	//シザー矩形を設定
	D3D12_RECT scissorRect{ 0, 0, WinApp::kWindowWidth,WinApp::kWindowHeight };
	Engine::GetList()->RSSetViewports(1, &viewport);
	Engine::GetList()->RSSetScissorRects(1, &scissorRect);

	// 三角形の描画
	Engine::GetList()->DrawInstanced(3, 1, 0, 0);

	PostDraw();

}

void GasianBlur::PreDraw() {
	//バリアの設定
	Engine::GetInstance()->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//レンダーターゲット
	Engine::GetList()->OMSetRenderTargets(1, &colorBuffer_->GetRTVHandle(), false, nullptr);
	//レンダーターゲットのクリア
	Engine::GetList()->ClearRenderTargetView(colorBuffer_->GetRTVHandle(), clearColor_, 0, nullptr);

}

void GasianBlur::PostDraw() {
	//バリアの変更
	Engine::GetInstance()->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

}

void GasianBlur::CreateResource() {
	resource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(GasianBlur));
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&blurData));
	blurData->stepWidth = 0.001f;
	blurData->sigma = 0.005f;
	
}

void GasianBlur::sPipeline() {
	GraphicsPipeline::GetInstance()->CreatePostEffectVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePostEffectPSShader(L"GasianBlur.PS.hlsl");


	rootSignature_ = GraphicsPipeline::GetInstance()->CreatePostEffectRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreatePostEffectGraphicsPipeline(pixelShaderBlob_);

}

