#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include <d3dx12.h>

class PostEffects
{
public:
	static PostEffects* GetInstance();

	static void Destroy();

	//void Initialize();

	void Update();

	void Draw();



private:
	PostEffects() = default;
	~PostEffects() = default;
	PostEffects(const PostEffects&) = delete;
	PostEffects& operator=(const PostEffects&) = delete;

	void CreateVertexBuffer();

	void CreatePipelineState();

private:
	static PostEffects* instance_;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;

	bool isEnable_ = false;
};