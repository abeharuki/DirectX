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
}

void OutLine::Update() {}

void OutLine::Draw() {
	
	
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Engine::GetList()->SetGraphicsRootDescriptorTable(0, Engine::GetInstance()->GetHandle());
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, Engine::GetInstance()->GetDepthHandle());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, postEffectResource_->GetGPUVirtualAddress());
	
	// 三角形の描画
	Engine::GetList()->DrawInstanced(3, 1, 0, 0);
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