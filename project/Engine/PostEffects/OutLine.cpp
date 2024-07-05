#include "OutLine.h"


void OutLine::Initialize() {
	viewProjection_.Initialize();
	sPipeline();
	CreateResource();

	colorBuffer_ = std::make_unique<ColorBuffer>();
	colorBuffer_->Create(WinApp::kWindowWidth, WinApp::kWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	
	
}

void OutLine::Draw() {

	PreDraw();

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Engine::GetList()->SetGraphicsRootDescriptorTable(0, Engine::GetInstance()->GetHandle());
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, Engine::GetInstance()->GetDepthHandle());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, postEffectResource_->GetGPUVirtualAddress());
	
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

void OutLine::PreDraw(){
	//バリアの設定
	Engine::GetInstance()->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//レンダーターゲット
	Engine::GetList()->OMSetRenderTargets(1, &colorBuffer_->GetRTVHandle(), false, nullptr);
	//レンダーターゲットのクリア
	Engine::GetList()->ClearRenderTargetView(colorBuffer_->GetRTVHandle(), clearColor_, 0, nullptr);
	//ディプス
	Engine::DepthPreDraw();
}

void OutLine::PostDraw(){
	//バリアの変更
	Engine::GetInstance()->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	//
	Engine::DepthPostDraw();
}

void OutLine::CreateResource() {
	postEffectResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(OutLine));
	postEffectResource_->Map(0, nullptr, reinterpret_cast<void**>(&outLineData));
	outLineData->isEnable = false;
	outLineData->differenceValue = 1.0f;
	outLineData->projectionInverse = Math::MakeIdentity4x4();
	outLineData->projectionInverse = Math::Inverse(viewProjection_.matProjection);
}

void OutLine::sPipeline() {

	GraphicsPipeline::GetInstance()->CreateOutLinePSShader();


	rootSignature_ = GraphicsPipeline::GetInstance()->CreateOutLineRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateOutLineGraphicsPipeline();

}
