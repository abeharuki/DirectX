#include "PostEffect.h"

PostEffect* PostEffect::instance_ = nullptr;

PostEffect* PostEffect::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new PostEffect();
	}
	return instance_;
}

void PostEffect::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}

}


void PostEffect::Initialize() {
	sPipeline();
	CreateResource();
}

void PostEffect::Draw() {

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Engine::GetList()->SetGraphicsRootDescriptorTable(0, Engine::GetInstance()->GetHandle());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, grayResource_->GetGPUVirtualAddress());

	// 三角形の描画
	Engine::GetList()->DrawInstanced(3, 1, 0, 0);

}

void PostEffect::CreateResource() {
	grayResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(Grayscale));
	grayResource_->Map(0, nullptr, reinterpret_cast<void**>(&grayData));
	grayData->isEnable = false;
}

void PostEffect::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePostEffectVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePostEffectPSShader();


	rootSignature_ = GraphicsPipeline::GetInstance()->CreatePostEffectRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreatePostEffectGraphicsPipeline();

};