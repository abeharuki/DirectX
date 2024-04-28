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
	//CreateVertexResource();
}

void PostEffect::Draw() {

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	
	//Engine::GetList()->SetGraphicsRootDescriptorTable(0, Engine::GetInstance()->GetHandle());
	Engine::GetList()->SetGraphicsRootDescriptorTable(0, Engine::GetInstance()->GetSRVHandle());
	// 三角形の描画
	Engine::GetList()->DrawInstanced(3, 1, 0, 0);

}

//頂点データの設定
void PostEffect::CreateVertexResource() {

	vertexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) * 3);
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得


	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * 3); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ	

	vertexData[0].position = { -1.0f,1.0f,0.0f,1.0f };
	vertexData[1].position = { 3.0f,1.0f,0.0f,1.0f };
	vertexData[2].position = { -1.0f,-3.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0,0 };
	vertexData[1].texcoord = { 2,0 };
	vertexData[2].texcoord = { 0,2 };

	
};

void PostEffect::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePostEffectVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePostEffectPSShader();


	rootSignature_ = GraphicsPipeline::GetInstance()->CreatePostEffectRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreatePostEffectGraphicsPipeline();

};
