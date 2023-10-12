#include "Sprite.h"
#include "GraphicsPipeline.h"
#include "TextureManeger.h"
#include "Engine.h"
#include "WorldTransform.h"


// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> Sprite::rootSignature_;
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> Sprite::sPipelineState_;
Microsoft::WRL::ComPtr<IDxcBlob> Sprite::vertexShaderBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> Sprite::pixelShaderBlob_;
// ライティング
Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::lightResource_;
// マテリアル用リソース
Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::materialResorce_;


Transform Sprite::uvTransformSprite{
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f},
};

Sprite* Sprite::GetInstance() {
	static Sprite instance;
	return &instance;
}


void Sprite::Initialize() {
	
	
}


void Sprite::Create(const std::string& fileName/* Vector4 color = {1, 1, 1, 1},
                    Vector2 anchorpoint = {0.0f, 0.0f}, bool isFlipX = false, bool isFlipY = false */) {



	LoadTexture(fileName);
	CreateVertexResource();
	sPipeline();
	
	

}

void Sprite::SetPosition(const Vector2& position) {
	
}

void Sprite::sPipeline(){


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

void Sprite::PreDraw() {

	
	
};

void Sprite::PostDraw(){};

void Sprite::Draw(WorldTransform& worldTransform, Sprite* sprite) {

	
	// Sprite用のworldViewProjectionMatrixを作る
	Matrix4x4 worldMatrixSprite = Math::MakeAffineMatrix(
	    {worldTransform.scale.x * static_cast<float>(sprite->textureWidth),
	     worldTransform.scale.y * static_cast<float>(sprite->textureHeight), 1.0f},
	    {0.0f, 0.0f, worldTransform.rotate.z},
	    {worldTransform.translate.x, worldTransform.translate.y, 0.5f});

	//  コマンドを積む
	Engine::GetList()->RSSetViewports(1, &sprite->viewport);
	Engine::GetList()->RSSetScissorRects(1, &sprite->scissorRect);


	//// UVTransform用の行列
	// Matrix4x4 uvTransformMatrix = Math::MakeAffineMatrix(
	//     uvTransformSprite.scale, uvTransformSprite.rotate, uvTransformSprite.translate);
	//
	// materialDataSprite->uvTransform = uvTransformMatrix;

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// Spriteをインデックス描画。
	Engine::GetList()->IASetVertexBuffers(0, 1, &sprite->vbView_); // VBVを設定
	Engine::GetList()->IASetIndexBuffer(&sprite->ibView_);         // IBVを設定

	
	// マテリアルCBufferの場所を設定
	//Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	//Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());

	Engine::GetList()->SetDescriptorHeaps(1, &sprite->SRVHeap);
	// TransformationMatrixCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootDescriptorTable(
	    2, sprite->SRVHeap->GetGPUDescriptorHandleForHeapStart());
	Engine::GetList()->SetGraphicsRootConstantBufferView(
	    1, worldTransform.constBuff_->GetGPUVirtualAddress());

	
	// 描画
	Engine::GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	
}


void Sprite::CreateVertexResource() {

	
	// バーテックス
	/*vertexData [4] = {
	    {{0.0f, 360.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	    {{0.0f, 0.0f, 0.0f, 1.0f},   {0.0f, 0.0f}},
	    {{640.0f, 360.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	    {{640.0f, 0.0f, 0.0f, 1.0f},   {1.0f, 1.0f}},
	};*/

	//VertexData* vertexData = nullptr;
	vertexResource_ = Mesh::CreateBufferResoure(Engine::GetDevice(), sizeof(VertexData) * 4);

	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(VertexData);


	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	
	// 1枚目の三角形
	vertexData_[0].position = {0.0f, 360.0f, 0.0f, 1.0f}; // 左下
	vertexData_[0].texcoord = {0.0f, 1.0f};
	vertexData_[1].position = {0.0f, 0.0f, 0.0f, 1.0f}; // 左上
	vertexData_[1].texcoord = {0.0f, 0.0f};
	vertexData_[2].position = {640.0f, 360.0f, 0.0f, 1.0f}; // 右下
	vertexData_[2].texcoord = {1.0f, 1.0f};
	vertexData_[3].position = {640.0f, 0.0f, 0.0f, 1.0f}; // 右上
	vertexData_[3].texcoord = {1.0f, 0.0f};

	//vertexDataSprite[0]->normal = {0.0f, 0.0f, -1.0f};
	

	
	// インデックス
	indexResource_ = Mesh::CreateBufferResoure(Engine::GetDevice(), sizeof(uint32_t) * 6);
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	

	indexData_[0] = {0};
	indexData_[1] = {1};
	indexData_[2] = {2};
	indexData_[3] = {1};
	indexData_[4] = {3};
	indexData_[5] = {2};


	// 頂点バッファビューを作成する
	// リソースの先頭アドレスから使う
	ibView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	ibView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	ibView_.Format = DXGI_FORMAT_R32_UINT;

	
	
	//// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	////TransformationMatrix wvpData;
	//wvpResource_ = Mesh::CreateBufferResoure(Engine::GetDevice(), sizeof(TransformationMatrix));
	//// データを書き込む
	//// 書き込むためのアドレスを取得
	//wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//// 単位行列を書き込む
	//*wvpData_ = Math::MakeIdentity4x4();
	
	//
	//// Sprite用のマテリアルリソースを作る
	//// マテリアル
	//materialResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice(), sizeof(Material));
	//// データを書き込む
	//// 書き込むためのアドレスを取得
	//materialResorce_->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	//// 今回は白を書き込む
	//materialDataSprite->color = {1.0f, 1.0f, 1.0f, 1.0f};
	//// Lightingを無効
	//materialDataSprite->enableLighting = false;
	//// 初期化
	//materialDataSprite->uvTransform = Math::MakeIdentity4x4();

}


// 画像の読み込み
void Sprite::LoadTexture(const std::string& fileName) {

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = TextureManager::LoadTexture(fileName);
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

	SRVHeap = CreateDescriptorHeap(Engine::GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10, true);


	// SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	//textureSrvHandleGPU = SRVHeap->GetGPUDescriptorHandleForHeapStart();

	//// 先頭はImGuiが使っているのでその次を使う
	//textureSrvHandleCPU.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(
	//    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(
	//    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	// SRVの作成
	Engine::GetDevice()->CreateShaderResourceView(
	    textureResource, &srvDesc, SRVHeap->GetCPUDescriptorHandleForHeapStart());
}


