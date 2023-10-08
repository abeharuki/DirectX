#include "Sprite.h"
#include "GraphicsPipeline.h"
#include "TextureManeger.h"
#include "Engine.h"


// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> Sprite::rootSignature_;
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> Sprite::sPipelineState_;
Microsoft::WRL::ComPtr<IDxcBlob> Sprite::vertexShaderBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> Sprite::pixelShaderBlob_;
D3D12_VIEWPORT Sprite::viewport{};
D3D12_RECT Sprite::scissorRect{};
// 頂点バッファビュー
D3D12_VERTEX_BUFFER_VIEW Sprite::vbView_ = {};
D3D12_INDEX_BUFFER_VIEW Sprite::ibView_ = {};
// 頂点
Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::vertexResource_;
Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::indexResource_;
// ライティング
Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::lightResource_;
// WVP用リソース
Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::wvpResouce_;
// マテリアル用リソース
Microsoft::WRL::ComPtr<ID3D12Resource> Sprite::materialResorce_;
TransformationMatrix* Sprite::wvpData;
Material* Sprite::materialDataSprite;
Transform Sprite::transform = {
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}
};
Transform Sprite::cameraTransform = {
    {1.0f, 1.0f, 1.0f  },
    {0.0f, 0.0f, 0.0f  },
    {0.0f, 0.0f, -10.0f}
};
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
	mesh_ = new Mesh;
	sPipeline();
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
	
	
	
	CreateVertexResource();
	
	LoadTexture();

};



void Sprite::PreDraw() {
	// Sprite用のworldViewProjectionMatrixを作る
	Matrix4x4 worldMatrixSprite = Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrixSprite = Math::MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = Math::MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Math::Multiply(worldMatrixSprite, Math::Multiply(viewMatrixSprite, projectionMatrixSprite));
	*wvpData = TransformationMatrix(worldViewProjectionMatrixSprite, worldMatrixSprite);
			
	//// UVTransform用の行列
	//Matrix4x4 uvTransformMatrix = Math::MakeAffineMatrix(
	//    uvTransformSprite.scale, uvTransformSprite.rotate, uvTransformSprite.translate);
	//
	//materialDataSprite->uvTransform = uvTransformMatrix;
	
	//  コマンドを積む
	Engine::GetList()->RSSetViewports(1, &viewport);
	Engine::GetList()->RSSetScissorRects(1, &scissorRect);
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// Spriteをインデックス描画。
	Engine::GetList()->IASetVertexBuffers(0, 1, &vbView_); // VBVを設定
	Engine::GetList()->IASetIndexBuffer(&ibView_); // IBVを設定

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
};

void Sprite::PostDraw(){};

void Sprite::Draw() {
	
	// マテリアルCBufferの場所を設定
	//Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	//Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());

	// TransformationMatrixCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, wvpResouce_->GetGPUVirtualAddress());

	
	// 描画
	// commandList->DrawInstanced(6, 1, 0, 0);
	Engine::GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	
}


void Sprite::CreateVertexResource() {

	mesh_->CreateBuffers(Engine::GetDevice());
	//バーテックス
	vbView_ = mesh_->GetVBViewSprite();
	// インデックス
	ibView_ = mesh_->GetIBView();

	vertexResource_ = mesh_->GetSprite();
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

	// 1枚目の三角形
	vertexDataSprite[0].position = {0.0f, 360.0f, 0.0f, 1.0f}; // 左下
	vertexDataSprite[0].texcoord = {0.0f, 1.0f};
	vertexDataSprite[1].position = {0.0f, 0.0f, 0.0f, 1.0f}; // 左上
	vertexDataSprite[1].texcoord = {0.0f, 0.0f};
	vertexDataSprite[2].position = {640.0f, 360.0f, 0.0f, 1.0f}; // 右下
	vertexDataSprite[2].texcoord = {1.0f, 1.0f};
	vertexDataSprite[3].position = {640.0f, 0.0f, 0.0f, 1.0f}; // 右上
	vertexDataSprite[3].texcoord = {1.0f, 0.0f};

	vertexDataSprite[0].normal = {0.0f, 0.0f, -1.0f};

	indexResource_ = mesh_->GetIndex();
	// データを書き込む
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;


	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResouce_ = mesh_->GetWVP();
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResouce_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = Math::MakeIdentity4x4();
	wvpData->World = Math::MakeIdentity4x4();

	
	// Sprite用のマテリアルリソースを作る
	// マテリアル
	materialResorce_ = mesh_->GetMaterial();
	// データを書き込む
	// 書き込むためのアドレスを取得
	materialResorce_->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	// 今回は白を書き込む
	materialDataSprite->color = {1.0f, 1.0f, 1.0f, 1.0f};
	// Lightingを無効
	materialDataSprite->enableLighting = false;
	// 初期化
	materialDataSprite->uvTransform = Math::MakeIdentity4x4();

}

// 画像の読み込み
void Sprite::LoadTexture() {

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

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU =
	    Engine::GetSRV()->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU = Engine::GetSRV()->GetGPUDescriptorHandleForHeapStart();

	// 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(
	    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(
	    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// SRVの作成
	Engine::GetDevice()->CreateShaderResourceView(
	    textureResource, &srvDesc, textureSrvHandleCPU);
}
