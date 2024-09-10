#include "Sprite.h"








Sprite* Sprite::GetInstance() {
	static Sprite instance;
	return &instance;
}


void Sprite::Initialize(const std::string& fileName) {
	LoadTexture(fileName);
	AdjustTextureSize();
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


	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateSpriteVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreateSpritePSShader();


	rootSignature_ = GraphicsPipeline::GetInstance()->CreateSpriteRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateSpritePipeline(blendMode_);

	
	

};

void Sprite::PreDraw() {

	
	
};

void Sprite::PostDraw(){};

void Sprite::Draw(Transform& uvTransform ) {
	
	UpdateVertexBuffer();
	
	Transform cameraTransform{
	    {1.0f, 1.0f, 1.0f  },
        {0.0f, 0.0f, 0.0f  },
        {0.0f, 0.0f, -10.0f}
    };
	
	// Sprite用のworldViewProjectionMatrixを作る
	Matrix4x4 matWorld_ = Math::MakeAffineMatrix(Vector3{size_.x,size_.y, 1.0f}, Vector3{0.0f, 0.0f,rotation_},Vector3{position_.x, position_.y, 0.5f});
	Matrix4x4 viewMatrixSprite = Math::MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = Math::MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Math::Multiply(matWorld_, Math::Multiply(viewMatrixSprite, projectionMatrixSprite));
	*wvpData = TransformationMatrix(worldViewProjectionMatrixSprite,matWorld_);

	

	// UVTransform用の行列
	Matrix4x4 uvTransformMatrix = Math::MakeAffineMatrix(
	    {
	        uvTransform.scale.x+1,
	        uvTransform.scale.y+1,
	        uvTransform.scale.z+1,
	    },
	    uvTransform.rotate, uvTransform.translate);
	
	 materialDataSprite->uvTransform = uvTransformMatrix;

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// Spriteをインデックス描画。
	Engine::GetList()->IASetVertexBuffers(0, 1, &vbView_); // VBVを設定
	Engine::GetList()->IASetIndexBuffer(&ibView_);         // IBVを設定

	
	
	//Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());
	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	// TransformationMatrixCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(texture_));
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, wvpResouce->GetGPUVirtualAddress());

	
	// 描画
	Engine::GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

	
}
void Sprite::Draw() {

	UpdateVertexBuffer();

	Transform cameraTransform{
		{1.0f, 1.0f, 1.0f  },
		{0.0f, 0.0f, 0.0f  },
		{0.0f, 0.0f, -10.0f}
	};

	// Sprite用のworldViewProjectionMatrixを作る
	Matrix4x4 matWorld_ = Math::MakeAffineMatrix(Vector3{ size_.x,size_.y, 1.0f }, Vector3{ 0.0f, 0.0f,rotation_ }, Vector3{ position_.x, position_.y, 0.5f });
	Matrix4x4 viewMatrixSprite = Math::MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = Math::MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Math::Multiply(matWorld_, Math::Multiply(viewMatrixSprite, projectionMatrixSprite));
	*wvpData = TransformationMatrix(worldViewProjectionMatrixSprite, matWorld_);



	// UVTransform用の行列
	Matrix4x4 uvTransformMatrix = Math::MakeAffineMatrix(
		{
			uv.scale.x + 1,
			uv.scale.y + 1,
			uv.scale.z + 1,
		},
		uv.rotate, uv.translate);

	materialDataSprite->uvTransform = uvTransformMatrix;

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// Spriteをインデックス描画。
	Engine::GetList()->IASetVertexBuffers(0, 1, &vbView_); // VBVを設定
	Engine::GetList()->IASetIndexBuffer(&ibView_);         // IBVを設定



	//Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());
	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	// TransformationMatrixCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(texture_));
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, wvpResouce->GetGPUVirtualAddress());


	// 描画
	Engine::GetList()->DrawIndexedInstanced(6, 1, 0, 0, 0);


}

void Sprite::UpdateVertexBuffer() {

	float left = (0.0f - anchorPoint_.x);
	float right = (1.0f - anchorPoint_.x);
	float top = (0.0f - anchorPoint_.y);
	float bottom = (1.0f - anchorPoint_.y);

	//左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	//上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	// 
	vertexData_[0].position = { left , bottom, 0.0f, 1.0f }; // 左下0
	vertexData_[0].texcoord = { 0.0f, 1.0f };
	vertexData_[1].position = { left , top, 0.0f, 1.0f }; // 左上1
	vertexData_[1].texcoord = { 0.0f, 0.0f };
	vertexData_[2].position = { right, bottom, 0.0f, 1.0f }; // 右下2
	vertexData_[2].texcoord = { 1.0f, 1.0f };
	vertexData_[3].position = { right, top, 0.0f, 1.0f }; // 右上3
	vertexData_[3].texcoord = { 1.0f, 0.0f };
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
	

	//vertexData_[0].normal = {0.0f, 0.0f, -1.0f};
	
	
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
	materialResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(MaterialD));
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
	TextureManager::Load(fileName);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(fileName);
	//textureManager_ = TextureManager::GetInstance();
	//textureManager_->Initialize();
	//texture_ = textureManager_->Load(fileName);
}

void Sprite::AdjustTextureSize()
{
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(texture_);
	size_ = { static_cast<float>(metadata.width),static_cast<float>(metadata.height) };
}


void Sprite::SetColor(Vector4 color) {
	materialDataSprite->color.rgb = {color.x, color.y, color.z};
	materialDataSprite->color.a = color.w;
}

void Sprite::SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }