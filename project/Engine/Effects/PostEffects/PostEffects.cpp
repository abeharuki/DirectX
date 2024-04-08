#include "PostEffects.h"



PostEffects* PostEffects::instance_ = nullptr;

PostEffects* PostEffects::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new PostEffects();
	}
	return instance_;
}

void PostEffects::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}

}
/*
void PostEffects::Initialize() {
	
	CreateVertexBuffer();
	CreatePipelineState();

	device_ = DirectXCommon::GetInstance()->GetDevice();

	HRESULT result;
	
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinApp::kWindowWidth,
		(UINT)WinApp::kWindowHeight,
		1,0,1,0,D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
			D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer));
	assert(SUCCEEDED(result));

	const UINT pixelCount = WinApp::kWindowWidth * WinApp::kWindowHeight;
	const UINT rowPitch = sizeof(UINT) * WinApp::kWindowWidth;
	const UINT depthPitch = rowPitch * WinApp::kWindowHeight;

	UINT* img = new UINT[pixelCount];
	for (int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

	result = vertexBuffer->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
	assert(SUCCEEDED(result));
	delete[] img;
	
	//SRV用ディスクリプタヒープ生成
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;

	result = DirectXCommon::GetInstance()->GetDevice()->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	//SRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(vertexBuffer.Get(),
		&srvDesc,
		descHeapSRV->GetCPUDescriptorHandleForHeapStart()
	);

}

void PostEffects::Draw() {


	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();


	
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(rootSignature_.Get());
	commandList->SetPipelineState(sPipelineState_.Get());

	// インデックス描画。
	commandList->IASetVertexBuffers(0, 1, &vbView_); // VBVを設定
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// TransformationMatrixCBufferの場所を設定
	commandList->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());

	

	// 描画
	commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);


}

void PostEffects::CreateVertexBuffer() {

	vertexData_[0].position = { 0.0f , 1.0f, 0.0f, 1.0f }; // 左下0
	vertexData_[0].texcoord = { 0.0f, 1.0f };
	vertexData_[1].position = { 0.0f , 0.0f, 0.0f, 1.0f }; // 左上1
	vertexData_[1].texcoord = { 0.0f, 0.0f };
	vertexData_[2].position = { 1.0f, 1.0f, 0.0f, 1.0f }; // 右下2
	vertexData_[2].texcoord = { 1.0f, 1.0f };
	vertexData_[3].position = { 1.0f, 0.0f, 0.0f, 1.0f }; // 右上3
	vertexData_[3].texcoord = { 1.0f, 0.0f };

	vertexBuffer = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) * 4);
	vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));


	//  リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(VertexData);
}


void PostEffects::CreatePipelineState() {
	rootSignature_ = GraphicsPipeline::GetInstance()->CreateRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateSpritePipeline(blendMode_);
}


*/