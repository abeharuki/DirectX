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
	//GaussianBlurの開放
	//GaussianBlur::Destroy();
}

void PostEffects::Initialize() {
	
	HRESULT result;
	
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinApp::kWindowWidth,
		WinApp::kWindowHeight,
		1,0,1,0,D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,D3D12_MEMORY_POOL_L0),
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






}