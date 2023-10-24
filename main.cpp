#include <Windows.h>
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>

#include <vector>
#include <fstream>
#include <sstream>
#include <wrl.h>
#include "Math.h"

#include "WinApp.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "Model.h"
#include "ImGuiManager.h"
#include "Engine.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxcompiler.lib")






// WIndowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;
	Engine::Initialize("自作エンジン", windowWidth, windowHeight);

	
	
	/*

	ID3D12Device* device = nullptr;

	
	
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	

	//RTV用のヒープでディスクリプタの数は2。RTVはShader内で触れるものではない
	ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものではないので、ShaderVisibleはtrue
	ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);


	
	

	//球
	
	

	// インデックスを使った球
	// インデックス用の頂点リソースを作る
	ID3D12Resource* indexResource = CreateBufferResoure(device, sizeof(uint32_t) * 1536);
	// 頂点バッファビューを作成する
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	// リソースの先頭アドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 1536;
	// インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// データを書き込む
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));


	

	
    /*
	
	
	
	
	
	

	
	

	
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	ID3D12Resource* materialResorce = CreateBufferResoure(device, sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResorce->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//今回は白を書き込む
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//Lightingを有効にする
	materialData->enableLighting = true;
	//初期化
	materialData->uvTransform = MakeIdentity4x4();
	
	
	
	

	//デフォルト値
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;
	
	
	
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	

	// 2枚目のTextrueを読んで転送する
	DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ID3D12Resource* textureResource2 = CreateTextureResource(device, metadata2);
	UploadTextureData(textureResource2, mipImages2);

	//objファイル
	/*
	/2枚目のTextrueを読んで転送する
	DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ID3D12Resource* textureResource2 = CreateTextureResource(device, metadata2);
	UploadTextureData(textureResource2, mipImages2);
	*/
	/*
	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	//先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVの作成
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);


	//2つ目のmetadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	const uint32_t desriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t desriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t desriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//2つ目のSRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, 2);
	//SRVの作成
	device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);

	

	

	


	
	bool useMonsterBall = true;
	*/

   
	// ウインドウの×ボタンが押されるまでループ
	while (true) {
		
		// メッセージ処理
		if (Engine::BeginFrame()) {break;}

		/*
			
			Math* math_;
			
			// ゲームの処理
			//transform.rotate.y += 0.03f;
			Matrix4x4 worldMatrix = math_->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = math_->MakeAffineMatrix(
			    cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = math_->Inverse(cameraMatrix);
			Matrix4x4 projecttionMatrix =
			    math_->MakePerspectiverFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
			//WVPMatrixを作る。同次クリップ空間
			Matrix4x4 worldViewProjectionMatrix =
			    math_->Multiply(worldMatrix, math_->Multiply(viewMatrix, projecttionMatrix));
			wvpData->WVP = worldViewProjectionMatrix;

		
			//マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResorce->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
			
			//SRVのDescriptorTableの先頭の設定。2はrootParameter[2]である
			commandList->SetGraphicsRootDescriptorTable(2,useMonsterBall ? textureSrvHandleGPU2 :  textureSrvHandleGPU);
			//wvp用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResouce->GetGPUVirtualAddress());
			commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
			//球の描画
			commandList->DrawInstanced(1536, 1, 0, 0);
			//commandList->DrawIndexedInstanced(1536, 1, 0, 0, 0);
			//commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

			//CBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			
			

			ImGui::Begin("Settings");
			ImGui::SliderFloat3("color", &materialData->color.x, 0.0f, 1.0f);
			//Sprite
			ImGui::SliderFloat3("translationSprite", &transformSprite.translate.x, 0.0f, 1280.0f);
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			//LightLight
			ImGui::SliderFloat3("LightColor", &directionalLightData->color.x, -1.0f,1.0f);
			ImGui::SliderFloat3("LightDirecton", &directionalLightData->direction.x, -1.0f,1.0f);
			ImGui::DragFloat("Intensity", &directionalLightData->intensity, 0.1f);
			//UV
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f,10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f,-10.0f,10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);


			ImGui::DragFloat3("Sphere.Rotate", &transform.rotate.x, 0.01f, -10.0f, 10.0f);
			ImGui::End();

			
			



			*/
		Engine::EndFrame();
		
	}
	
	/*
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	
	//解放処理
	ResourceObject deptStencilResource =
	    CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);
	CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
	
#ifdef _DEBUG
	debugController->Release();
#endif
	CloseWindow(hwnd);
	
	srvDescriptorHeap->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();
	materialResorce->Release();
	wvpResouce->Release();
	materialResourceSprite->Release();
	directionalLightResource->Release();
	transformationMatrixResourceSprite->Release();
	vertexResource->Release();
	vertexResourceSprite->Release();
	indexResourceSprite->Release();
	textureResource->Release();
	depthStencilResource->Release();
	dsvDescriptorHeap->Release();
	textureResource2->Release();
	indexResource->Release();

	
	}
	*/
	
	//リソースリークチェック
	//dxCommon->Debug();
	Engine::Finalize();
	

	return 0;
}



