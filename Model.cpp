#include "Model.h"
#include <cassert>
#include <format>

#include "StringUtility.h"
#include "GraphicsPipeline.h"
#include "Engine.h"
#include "TextureManeger.h"
#include "externals/imgui/imgui.h"

// コマンドリスト
ID3D12GraphicsCommandList* Model::sCommandList_ = nullptr;
// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::rootSignature_;
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sPipelineState_;
Microsoft::WRL::ComPtr<IDxcBlob> Model::vertexShaderBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> Model::pixelShaderBlob_;
D3D12_VIEWPORT Model::viewport{};
D3D12_RECT Model::scissorRect{};
// 頂点バッファビュー
D3D12_VERTEX_BUFFER_VIEW Model::vbView_ = {};
D3D12_INDEX_BUFFER_VIEW Model::ibView_ = {};
// 頂点
Microsoft::WRL::ComPtr<ID3D12Resource> Model::vertexResource_;
// ライティング
Microsoft::WRL::ComPtr<ID3D12Resource> Model::lightResource_;
// WVP用リソース
Microsoft::WRL::ComPtr<ID3D12Resource> Model::wvpResouce_;
// マテリアル用リソース
Microsoft::WRL::ComPtr<ID3D12Resource> Model::materialResorce_;
TransformationMatrix* Model::wvpData;
Transform Model::transform = {
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}
};
Transform Model::cameraTransform = {
    {1.0f, 1.0f, 1.0f  },
    {0.0f, 0.0f, 0.0f  },
    {0.0f, 0.0f, -10.0f}
};
D3D12_GPU_DESCRIPTOR_HANDLE Model::textureSrvHandleGPU;


Model* Model::GetInstance() {
	static Model instance;
	return &instance;

}

void Model::Initialize(DirectXCommon* dxCommon) { 
	dxCommon_ = dxCommon;
	mesh_ = new Mesh;
	sPipeline();
}


void Model::sPipeline() {
	
	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePSShader();

	rootSignature_ = GraphicsPipeline::GetInstance()->CreateRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateGraphicsPipeline();

	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kWindowWidth;
	viewport.Height = WinApp::kWindowHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形

	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kWindowWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kWindowHeight;

	CreateVertexResource();

	LoadTexture();

	
};

void Model::PreDraw(ID3D12GraphicsCommandList* commandList) { 
	sCommandList_ = commandList;

	// ゲームの処理
	//transform.rotate.y += 0.03f;
	Matrix4x4 worldMatrix =
	    Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = Math::MakeAffineMatrix(
	     cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Math::Inverse(cameraMatrix);
	Matrix4x4 projecttionMatrix =
	    Math::MakePerspectiverFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	// WVPMatrixを作る。同次クリップ空間
	Matrix4x4 worldViewProjectionMatrix =
	    Math::Multiply(worldMatrix, Math::Multiply(viewMatrix, projecttionMatrix));
	wvpData->WVP = worldViewProjectionMatrix;
	

	// コマンドを積む
	sCommandList_->RSSetViewports(1, &viewport);
	sCommandList_->RSSetScissorRects(1, &scissorRect);
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	sCommandList_->SetGraphicsRootSignature(rootSignature_.Get());
	sCommandList_->SetPipelineState(sPipelineState_.Get());



	//sCommandList_->IASetIndexBuffer(&ibView_); // IBVを設定
	//sCommandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);

	
	
	
}

void Model::PostDraw() {
	// コマンドリストを解除
	Model::sCommandList_ = nullptr;
}

void Model::Draw() {

	sCommandList_->IASetVertexBuffers(0, 1, &vbView_);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	sCommandList_->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());

	Engine::GetList()->SetDescriptorHeaps(1, &SRVHeap);
	// SRVのDescriptorTableの先頭の設定。2はrootParameter[2]である
	sCommandList_->SetGraphicsRootDescriptorTable(2, SRVHeap->GetGPUDescriptorHandleForHeapStart());
	// wvp用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(1, wvpResouce_->GetGPUVirtualAddress());

	// 三角形の描画
	sCommandList_->DrawInstanced(6, 1, 0, 0);

	ImGui::Begin("Settings");
	ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat3("Translation", &transform.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f, -10.0f, 10.0f);
	ImGui::End();
}

//頂点データの設定
void Model::CreateVertexResource() {
	mesh_->CreateBuffers(dxCommon_->GetDevice());

	// バーテックス
	vbView_ = mesh_->GetVBView();
	vertexResource_ = mesh_->GetVertex();
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	// 左下
	vertexData[0].position = {-0.5f, -0.5f, 0.0f, 1.0f}; // 左下
	vertexData[0].texcoord = {0.0f, 1.0f};
	// 上
	vertexData[1].position = {0.0f, 0.5f, 0.0f, 1.0f}; // 左上
	vertexData[1].texcoord = {0.5f, 0.0f};
	// 右下
	vertexData[2].position = {0.5f, -0.5f, 0.0f, 1.0f}; // 右下
	vertexData[2].texcoord = {1.0f, 1.0f};

	vertexData[3].position = {-0.5f, -0.5f, 0.5f, 1.0f}; // 左上
	vertexData[3].texcoord = {0.0f, 1.0f};

	vertexData[4].position = {0.0f, 0.0f, 0.0f, 1.0f}; // 上
	vertexData[4].texcoord = {0.5f, 0.0f};

	vertexData[5].position = {0.5f, -0.5f, -0.5f, 1.0f}; // 右下
	vertexData[5].texcoord = {1.0f, 1.0f};


	// インデックス
	//ibView_ = mesh_->GetIBView();

	// WVP
	wvpResouce_ = mesh_->GetWVP();
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResouce_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = Math::MakeIdentity4x4();
	wvpData->World = Math::MakeIdentity4x4();

	// マテリアル
	materialResorce_ = mesh_->GetMaterial();

	// マテリアルにデータを書き込む
	// 書き込むためのアドレスを取得
	materialResorce_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 今回は白を書き込む
	materialData->color.rgb = Vector3(1.0f, 1.0f, 1.0f);
	materialData->color.a = float(1.0f);
	// Lightingを有効にする
	materialData->enableLighting = false;
	// 初期化
	materialData->uvTransform = Math::MakeIdentity4x4();

	// ライティング
	lightResource_ = mesh_->GetLight();
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// デフォルト値
	directionalLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	directionalLightData->direction = {0.0f, -1.0f, 0.0f};
	directionalLightData->intensity = 1.0f;

	
};

//画像の読み込み
void Model::LoadTexture() {
	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = TextureManager::LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource =
	    TextureManager::CreateTextureResource(Engine::GetDevice(), metadata);
	TextureManager::UploadTextureData(textureResource, mipImages);

	// metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//// SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU =
	//    Engine::GetSRV()->GetCPUDescriptorHandleForHeapStart();
	//textureSrvHandleGPU = Engine::GetSRV()->GetGPUDescriptorHandleForHeapStart();

	//// 先頭はImGuiが使っているのでその次を使う
	//textureSrvHandleCPU.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(
	//    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(
	//    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	SRVHeap = CreateDescriptorHeap(Engine::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10, true);




	// SRVの作成
	Engine::GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, SRVHeap->GetCPUDescriptorHandleForHeapStart());
}