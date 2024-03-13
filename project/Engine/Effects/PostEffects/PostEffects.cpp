#include "PostEffects.h"
/*


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
	/*
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
	
}*/