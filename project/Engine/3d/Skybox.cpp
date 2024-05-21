#include "Skybox.h"

void Skybox::Initialize(const std::string& texturePath) {
	worldTransform_.Initialize();
	worldTransform_.scale = { 10000,10000,10000 };
	worldTransform_.UpdateMatrix();
	LoadTexture(texturePath);
	CreateVertexResource();
	sPipeline();
}

void Skybox::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateSkyboxVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreateSkyboxPSShader();


	rootSignature_ = GraphicsPipeline::GetInstance()->CreateSkyboxRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateSkyboxGraphicsPipeline(blendMode_);

};

void Skybox::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection){
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	Engine::GetList()->IASetIndexBuffer(&indexBufferView);



	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(texture_));

	// wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, worldTransform_.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(3, viewProjection.constBuff_->GetGPUVirtualAddress());

	// 三角形の描画
	Engine::GetList()->DrawIndexedInstanced(36, 1, 0, 0,0);


}

Skybox* Skybox::Create(const std::string& texturePath) {
	Skybox* skybox = new Skybox;
	skybox->Initialize(texturePath);
	return skybox;


}


//頂点データの設定
void Skybox::CreateVertexResource() {
	// モデルの読み込み 
	// 頂点リソースを作る
	vertexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) * 24);
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得

	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * 24); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	//右面
	vertexData[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
	//左面
	vertexData[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData[5].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData[7].position = { -1.0f,-1.0f,1.0f,1.0f };
	//前方
	vertexData[8].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData[9].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[10].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData[11].position = { 1.0f,-1.0f,1.0f,1.0f };
	//後方
	vertexData[12].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[13].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData[14].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertexData[15].position = { -1.0f,-1.0f,-1.0f,1.0f };
	//上
	vertexData[16].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData[17].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[18].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData[19].position = { 1.0f,1.0f,1.0f,1.0f };
	//下
	vertexData[20].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData[21].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData[22].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData[23].position = { 1.0f,-1.0f,-1.0f,1.0f };
	
	indexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(uint32_t) * 36);

	// 頂点バッファビューを作成する
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 36; // 使用するリソースのサイズは頂点サイズ
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // 1頂点あたりのサイズ

	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData)); // 書き込むためのアドレスを取得
	//右面
	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 2; indexData[4] = 1; indexData[5] = 3;
	//左面
	indexData[6] = 4; indexData[7] = 5; indexData[8] = 6;
	indexData[9] = 6; indexData[10] = 5; indexData[11] = 7;
	//前面
	indexData[12] = 8; indexData[13] = 9; indexData[14] = 10;
	indexData[15] = 10; indexData[16] = 9; indexData[17] = 11;
	//後面
	indexData[18] = 12; indexData[19] = 13; indexData[20] = 14;
	indexData[21] = 14; indexData[22] = 13; indexData[23] = 15;
	//上面
	indexData[24] = 16; indexData[25] = 17; indexData[26] = 18;
	indexData[27] = 18; indexData[28] = 17; indexData[29] = 19;
	//下面
	indexData[30] = 20; indexData[31] = 21; indexData[32] = 22;
	indexData[33] = 22; indexData[34] = 21; indexData[35] = 23;


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
	//光沢
	materialData->shininess = 10.0f;
	// 初期化
	materialData->uvTransform = Math::MakeIdentity4x4();
};


void Skybox::LoadTexture(const std::string& texturePath) {
	TextureManager::GetInstance()->Load(texturePath);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(texturePath);
}