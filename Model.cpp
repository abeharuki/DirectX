#include "Model.h"
#include <cassert>
#include <format>

#include "StringUtility.h"
#include "GraphicsPipeline.h"
#include "externals/imgui/imgui.h"



// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::rootSignature_;
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sPipelineState_;
Microsoft::WRL::ComPtr<IDxcBlob> Model::vertexShaderBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> Model::pixelShaderBlob_;


void Model::Initialize(const std::string& filename, const std::string& texturePath) { 
	textureManager_ = TextureManager::GetInstance();
	modelData = TextureManager::LoadObjFile(filename);
	texture_ = textureManager_->Load(texturePath);
	CreateVertexResource();
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



	
};

//void Model::Draw(WorldTransform& worldTransform, Model* model) {
//	// Sprite用のworldViewProjectionMatrixを作る
//	Matrix4x4 viewMatrixSprite = Math::MakeIdentity4x4();
//	Matrix4x4 projectionMatrixSprite =
//	    Math::MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
//	Matrix4x4 worldViewProjectionMatrixSprite = Math::Multiply(
//	    worldTransform.matWorld_, Math::Multiply(viewMatrixSprite, projectionMatrixSprite));
//	worldTransform.matWorld_ = worldViewProjectionMatrixSprite;
//
//	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
//	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
//	Engine::GetList()->SetPipelineState(sPipelineState_.Get());
//
//	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
//	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	Engine::GetList()->IASetVertexBuffers(0, 1, &model->vertexBufferView);
//
//	// マテリアルCBufferの場所を設定
//	// Engine::GetList()->SetGraphicsRootConstantBufferView(0,
//	// model->materialResorce_->GetGPUVirtualAddress());
//	// Engine::GetList()->SetGraphicsRootConstantBufferView(3,
//	// lightResource_->GetGPUVirtualAddress());
//
//	Engine::GetList()->SetDescriptorHeaps(1, model->SRVHeap.GetAddressOf());
//	// SRVのDescriptorTableの先頭の設定。2はrootParameter[2]である
//	Engine::GetList()->SetGraphicsRootDescriptorTable(
//	    2, model->SRVHeap->GetGPUDescriptorHandleForHeapStart());
//	// wvp用のCBufferの場所を設定
//	Engine::GetList()->SetGraphicsRootConstantBufferView(
//	    1, worldTransform.constBuff_->GetGPUVirtualAddress());
//
//	// 三角形の描画
//	Engine::GetList()->DrawInstanced(UINT(model->modelData_.vertices.size()), 1, 0, 0);
//}
//	

void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection) {
	worldTransform.matWorld_ = Math::MakeAffineMatrix(
	    {worldTransform.scale.x, worldTransform.scale.y, 1.0f},
	    {0.0f, 0.0f, worldTransform.rotate.z},
	    {worldTransform.translate.x, worldTransform.translate.y, 0.5f});

	
	
	Matrix4x4 worldViewProjectionMatrixSprite = Math::Multiply(
	    worldTransform.matWorld_,
	    Math::Multiply(viewProjection.matView, viewProjection.matProjection));
	*wvpData = TransformationMatrix(worldViewProjectionMatrixSprite, worldTransform.matWorld_);

	//  コマンドを積む
	Engine::GetList()->RSSetViewports(1, &viewport);
	Engine::GetList()->RSSetScissorRects(1, &scissorRect);

	

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	
	
	//Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());

	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());

    // wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, wvpResouce->GetGPUVirtualAddress());

		// SRVのDescriptorTableの先頭の設定。2はrootParameter[2]である
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texture_));
	

	// 三角形の描画
	Engine::GetList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

	
}

//頂点データの設定
void Model::CreateVertexResource() {
	// モデルの読み込み 
	// 頂点リソースを作る
	vertexResource = Mesh::CreateBufferResoure(
	    Engine::GetDevice().Get(), sizeof(VertexData) * modelData.vertices.size());
	// 頂点バッファビューを作成する
	
	vertexBufferView.BufferLocation =vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT( sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ


	// 頂点リソースにデータを書き込む
	vertexResource->Map(
	    0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得
	std::memcpy( vertexData, modelData.vertices.data(),sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースにコピース
	//std::copy(modelData.vertices.begin(), modelData.vertices.end(), vertexData);

	//	重要
	//vertexResource->Unmap(0, nullptr);

	// マテリアル
	materialResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(Material));

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
	lightResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(DirectionalLight));
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// デフォルト値
	directionalLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	directionalLightData->direction = {0.0f, -1.0f, 0.0f};
	directionalLightData->intensity = 1.0f;

	wvpResouce = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(TransformationMatrix));
	// データを書き込む
	wvpData = nullptr;
	// 書き込むためのアドレスを取得
	wvpResouce->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = Math::MakeIdentity4x4();
};

//画像の読み込み
//void Model::LoadTexture(const std::string& filename, const std::string& texturePath) {
//	
//	// Textureを読んで転送する
//	DirectX::ScratchImage mipImages = TextureManager::LoadTexture(texturePath);
//	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
//	textureResource =TextureManager::CreateTextureResource(Engine::GetDevice(), metadata);
//	TextureManager::UploadTextureData(textureResource.Get(), mipImages);
//
//
//	SRVHeap = CreateDescriptorHeap(Engine::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10, true);
//
//
//	// metadataを基にSRVの設定
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//	srvDesc.Format = metadata.format;
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
//	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
//
//	//// SRVを作成するDescriptorHeapの場所を決める
//	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU =
//	//    Engine::GetSRV()->GetCPUDescriptorHandleForHeapStart();
//	//textureSrvHandleGPU = Engine::GetSRV()->GetGPUDescriptorHandleForHeapStart();
//
//	//// 先頭はImGuiが使っているのでその次を使う
//	//textureSrvHandleCPU.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(
//	//    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//	//textureSrvHandleGPU.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(
//	//    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//
//	
//
//	// SRVの作成
//	Engine::GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, SRVHeap->GetCPUDescriptorHandleForHeapStart());
//	textureSrvHandleGPU = GetGPUDescriptorHandle(SRVHeap.Get(), Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), 0);
//}



Model* Model::CreateModelFromObj(const std::string& filename, const std::string& texturePath) {
	Model* model = new Model;
	model->Initialize(filename, texturePath);
	return model;


}

void Model::PreDraw() {}

void Model::PostDraw() {}


