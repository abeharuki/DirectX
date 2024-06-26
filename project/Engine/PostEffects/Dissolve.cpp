#include "Dissolve.h"
#include <TextureManeger.h>

Dissolve* Dissolve::instance_ = nullptr;

Dissolve* Dissolve::GetInstance(){
	if (instance_ == nullptr)
	{
		instance_ = new Dissolve();
	}
	return instance_;
}

void Dissolve::Destroy(){
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void Dissolve::Initialize(){
	TextureManager::GetInstance()->Load("resources/Mask/noise0.png");
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/Mask/noise0.png");
	sPipeline();
	CreateResource();

	colorBuffer_ = std::make_unique<ColorBuffer>();
	colorBuffer_->Create(WinApp::kWindowWidth, WinApp::kWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

}

void Dissolve::Draw(DescriptorHandle srvHandle){
	PreDraw();

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Engine::GetList()->SetGraphicsRootDescriptorTable(0, srvHandle);
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(texture_));
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

void Dissolve::PreDraw(){
	//バリアの設定
	Engine::GetInstance()->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//レンダーターゲット
	Engine::GetList()->OMSetRenderTargets(1, &colorBuffer_->GetRTVHandle(), false, nullptr);
	//レンダーターゲットのクリア
	Engine::GetList()->ClearRenderTargetView(colorBuffer_->GetRTVHandle(), clearColor_, 0, nullptr);

}

void Dissolve::PostDraw(){
	//バリアの変更
	Engine::GetInstance()->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

}

void Dissolve::CreateResource(){
	resource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(Dissolve));
	resource_->Map(0, nullptr, reinterpret_cast<void**>(&dissolveData));
	dissolveData->isEnble = false;
	dissolveData->threshold = 1.0f;
	dissolveData->edgeColor = { 1.0f,0.4f,0.3f };
}

void Dissolve::sPipeline(){
	GraphicsPipeline::GetInstance()->CreateDissolvePSShader();


	rootSignature_ = GraphicsPipeline::GetInstance()->CreateDissolveRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateDissolveGraphicsPipeline();
}

void Dissolve::LoadTexture(const std::string& texturePath) {

	//テクスチャを設定
	if ("resources/Mask/" + texturePath != "")
	{
		TextureManager::GetInstance()->Load("resources/Mask/" + texturePath);
		texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/Mask/" + texturePath);
	}
	else
	{
		texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/Mask/noise0.png");
	}

}
