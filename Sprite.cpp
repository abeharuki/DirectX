#include "Sprite.h"
#include "GraphicsPipeline.h"
#include "TextureManeger.h"
#include "Engine.h"
#include "WorldTransform.h"
#include "externals/imgui/imgui.h"


// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> Sprite::rootSignature_;
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> Sprite::sPipelineState_;
Microsoft::WRL::ComPtr<IDxcBlob> Sprite::vertexShaderBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> Sprite::pixelShaderBlob_;




Sprite* Sprite::GetInstance() {
	static Sprite instance;
	return &instance;
}


void Sprite::Initialize(const std::string& fileName) {
	LoadTexture(fileName);
	CreateVertexResource();
	sPipeline();
}


Sprite* Sprite::Create(const std::string& fileName/* Vector4 color = {1, 1, 1, 1},
                    Vector2 anchorpoint = {0.0f, 0.0f}, bool isFlipX = false, bool isFlipY = false */) {
	Sprite* sprite = new Sprite;
	sprite->Initialize(fileName);
	return sprite;

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

void Sprite::Draw(WorldTransform& worldTransform) {
	
	
	// transform.rotate.y += 0.03f;
	
	Transform cameraTransform{
	    {1.0f, 1.0f, 1.0f  },
        {0.0f, 0.0f, 0.0f  },
        {0.0f, 0.0f, -10.0f}
    };
	
	// Sprite用のworldViewProjectionMatrixを作る
	worldTransform.matWorld_ = Math::MakeAffineMatrix(
	    {worldTransform.scale.x,worldTransform.scale.y, 1.0f},
	    {0.0f, 0.0f, worldTransform.rotate.z},
	    {worldTransform.translate.x, worldTransform.translate.y, 0.5f});

	Matrix4x4 viewMatrixSprite = Math::MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = Math::MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Math::Multiply(
	    worldTransform.matWorld_, Math::Multiply(viewMatrixSprite, projectionMatrixSprite));
	*wvpData = TransformationMatrix(worldViewProjectionMatrixSprite, worldTransform.matWorld_);

	//  コマンドを積む
	Engine::GetList()->RSSetViewports(1, &viewport);
	Engine::GetList()->RSSetScissorRects(1, &scissorRect);


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
	Engine::GetList()->IASetVertexBuffers(0, 1, &vbView_); // VBVを設定
	Engine::GetList()->IASetIndexBuffer(&ibView_);         // IBVを設定

	
	
	//Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());
	Engine::GetList()->SetDescriptorHeaps(1, &SRVHeap);
	// TransformationMatrixCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootDescriptorTable( 2, SRVHeap->GetGPUDescriptorHandleForHeapStart());
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, wvpResouce->GetGPUVirtualAddress());

	
	// 描画
	Engine::GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	
}


void Sprite::CreateVertexResource() {


	//VertexData* vertexData = nullptr;
	vertexResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData)*4);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	

	//  リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(VertexData);
	
	

	// 1枚目の三角形
	vertexData_[0].position = {0.0f, 360.0f, 0.0f, 1.0f}; // 左下0
	vertexData_[0].texcoord = {0.0f, 1.0f};
	vertexData_[1].position = {0.0f, 0.0f, 0.0f, 1.0f}; // 左上1
	vertexData_[1].texcoord = {0.0f, 0.0f};
	vertexData_[2].position = {640.0f, 360.0f, 0.0f, 1.0f}; // 右下2
	vertexData_[2].texcoord = {1.0f, 1.0f};
	vertexData_[3].position = {640.0f, 0.0f, 0.0f, 1.0f}; // 右上3
	vertexData_[3].texcoord = {1.0f, 0.0f};

	vertexData_[0].normal = {0.0f, 0.0f, -1.0f};
	
	
	// インデックス
	indexResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(uint32_t) * 6);
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	
	
	// 頂点バッファビューを作成する
	// リソースの先頭アドレスから使う
	ibView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	ibView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	ibView_.Format = DXGI_FORMAT_R32_UINT;

    indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 3;
	indexData_[5] = 2;
	

	
	wvpResouce = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(TransformationMatrix));
	// データを書き込む
	wvpData = nullptr;
	// 書き込むためのアドレスを取得
	wvpResouce->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = Math::MakeIdentity4x4();
	
	
	// Sprite用のマテリアルリソースを作る
	// マテリアル
	materialResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(Material));
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
void Sprite::LoadTexture(const std::string& fileName) {

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = TextureManager::LoadTexture(fileName);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* resource =
	    TextureManager::CreateTextureResource(Engine::GetDevice().Get(), metadata);
	TextureManager::UploadTextureData(resource, mipImages);

	
	//	画像サイズの代入
	textureWidth = static_cast<uint32_t>(metadata.width);
	textureHeight = static_cast<uint32_t>(metadata.height);

	SRVHeap = CreateDescriptorHeap(Engine::GetDevice().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10, true);

	// metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	
	// SRVの作成
	Engine::GetDevice()->CreateShaderResourceView(resource, &srvDesc, SRVHeap->GetCPUDescriptorHandleForHeapStart());
}


