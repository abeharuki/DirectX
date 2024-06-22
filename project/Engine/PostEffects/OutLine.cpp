#include "OutLine.h"

OutLine* OutLine::instance_ = nullptr;

OutLine* OutLine::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new OutLine();
	}
	return instance_;
}

void OutLine::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}

}


void OutLine::Initialize() {
	sPipeline();
	CreateResource();
	CreateRenderTexture();
	
}

void OutLine::Update() {}

void OutLine::Draw() {

	Engine::GetInstance()->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);

	Engine::GetList()->OMSetRenderTargets(1, &colorBuffer_->GetRTVHandle(), false, nullptr);

	float clearColor_[4]{ 0.1f, 0.25f, 0.5f, 1.0f };
	Engine::GetList()->ClearRenderTargetView(colorBuffer_->GetRTVHandle(), clearColor_, 0, nullptr);

	Engine::DepthPreDraw();

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


	Engine::GetInstance()->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	Engine::DepthPostDraw();

}

void OutLine::Apply() {}

void OutLine::CreateResource() {
	postEffectResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(OutLine));
	postEffectResource_->Map(0, nullptr, reinterpret_cast<void**>(&outLineData));
	outLineData->isEnable = false;
	outLineData->differenceValue = 1.0f;
	outLineData->projectionInverse = Math::MakeIdentity4x4();
}

void OutLine::sPipeline() {

	GraphicsPipeline::GetInstance()->CreateOutLinePSShader();


	rootSignature_ = GraphicsPipeline::GetInstance()->CreateOutLineRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateOutLineGraphicsPipeline();

}

void OutLine::CreateRenderTexture()
{
	colorBuffer_ = std::make_unique<ColorBuffer>();
	colorBuffer_->Create(WinApp::kWindowWidth, WinApp::kWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);


	/*/ RTVの作成
	const Vector4 kRenderTargetClearValue{ 0.1f, 0.25f, 0.5f, 1.0f };
	renderTextureResource = DirectXCommon::GetInstance()->CreateRenderTextureResource(Engine::GetDevice().Get(), WinApp::kWindowWidth, WinApp::kWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	rtvHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	Engine::GetDevice().Get()->CreateRenderTargetView(renderTextureResource.Get(), &rtvDesc, rtvHandle_);

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	srvHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// SRVの作成
	Engine::GetDevice().Get()->CreateShaderResourceView(renderTextureResource.Get(), &renderTextureSrvDesc, srvHandle_);
	*/
}
