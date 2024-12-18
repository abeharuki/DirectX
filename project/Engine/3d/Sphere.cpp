#include "Sphere.h"
#include <cassert>
#include <format>
#include <imgui.h>
#include "Model.h"
#include <numbers> 

Microsoft::WRL::ComPtr<IDxcBlob> Sphere::vertexShaderBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> Sphere::pixelShaderBlob_;

//Microsoft::WRL::ComPtr<ID3D12Resource> Sphere::lightResource_;
//WritingStyle* Sphere::lightData;


void Sphere::Initialize(const std::string& texturePath) {
	LoadTexture(texturePath);
	CreateVertexResource();
	sPipeline();
}


void Sphere::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePSShader();

	rootSignature_ = GraphicsPipeline::GetInstance()->CreateRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateGraphicsPipeline(blendMode_);


};



void Sphere::Draw(
	WorldTransform& worldTransform, const ViewProjection& viewProjection, bool light) {
	cameraData->worldPos = viewProjection.worldPos_;


	// ライティング有効化
	materialData_->SetLighting(light);



	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);


	
	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(texture_));
	Engine::GetList()->SetGraphicsRootDescriptorTable(7, TextureManager::GetInstance()->GetGPUHandle(maskTexture_));


	// wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialData_->GetResource()->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(5, cameraResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());

	// 三角形の描画
	Engine::GetList()->DrawInstanced(vertexIndex, 1, 0, 0);
}

// 頂点データの設定
void Sphere::CreateVertexResource() {
	vertexIndex = (kSubdivision_ * kSubdivision_) * 6;


	// モデルの読み込み
	// 頂点リソースを作る
	vertexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) * vertexIndex);
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得

	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * vertexIndex); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	//const uint32_t kSubdivision = kSubdivision_; // 分割数
	const float pi = (float)std::numbers::pi;                 // π
	const float kLonEvery = 2.0f * pi / kSubdivision_; // 経度分割1つ分の角度(φd)
	const float kLatEvery = pi / kSubdivision_;        // 緯度分割1つ分の角度(θd)
	float u;
	float v;
	// 緯度の方向に分割-π/2~π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision_; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex; // 現在の緯度(θ)
		// 経度の方向に分割
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision_; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision_ + lonIndex) * 6;
			float lon = lonIndex * kLonEvery; // 現在の経度(φ)
			u = float(lonIndex) / float(kSubdivision_);
			v = 1.0f - float(latIndex) / float(kSubdivision_);
			float uv = 1.0f / float(kSubdivision_);
			// a 左下
			vertexData[start].position.x = cos(lat) * cos(lon);
			vertexData[start].position.y = sin(lat);
			vertexData[start].position.z = cos(lat) * sin(lon);
			vertexData[start].position.w = 1.0f;
			vertexData[start].texcoord = { u, v };
			// b 左上
			vertexData[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexData[start + 1].position.y = sin(lat + kLatEvery);
			vertexData[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexData[start + 1].position.w = 1.0f;
			vertexData[start + 1].texcoord = { u, v - uv };
			// c 右下
			vertexData[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexData[start + 2].position.y = sin(lat);
			vertexData[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexData[start + 2].position.w = 1.0f;
			vertexData[start + 2].texcoord = { u + uv, v };

			// b 左上
			vertexData[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			vertexData[start + 3].position.y = sin(lat + kLatEvery);
			vertexData[start + 3].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			vertexData[start + 3].position.w = 1.0f;
			vertexData[start + 3].texcoord = { u + uv, v - uv };
			// d 右上
			vertexData[start + 4].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexData[start + 4].position.y = sin(lat);
			vertexData[start + 4].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexData[start + 4].position.w = 1.0f;
			vertexData[start + 4].texcoord = { u + uv, v };
			// c 右下
			vertexData[start + 5].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexData[start + 5].position.y = sin(lat + kLatEvery);
			vertexData[start + 5].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexData[start + 5].position.w = 1.0f;
			vertexData[start + 5].texcoord = { u, v - uv };

			vertexData[start].normal.x = vertexData[start].position.x;
			vertexData[start].normal.y = vertexData[start].position.y;
			vertexData[start].normal.z = vertexData[start].position.z;

			vertexData[start + 1].normal.x = vertexData[start + 1].position.x;
			vertexData[start + 1].normal.y = vertexData[start + 1].position.y;
			vertexData[start + 1].normal.z = vertexData[start + 1].position.z;

			vertexData[start + 2].normal.x = vertexData[start + 2].position.x;
			vertexData[start + 2].normal.y = vertexData[start + 2].position.y;
			vertexData[start + 2].normal.z = vertexData[start + 2].position.z;

			vertexData[start + 3].normal.x = vertexData[start + 3].position.x;
			vertexData[start + 3].normal.y = vertexData[start + 3].position.y;
			vertexData[start + 3].normal.z = vertexData[start + 3].position.z;

			vertexData[start + 4].normal.x = vertexData[start + 4].position.x;
			vertexData[start + 4].normal.y = vertexData[start + 4].position.y;
			vertexData[start + 4].normal.z = vertexData[start + 4].position.z;

			vertexData[start + 5].normal.x = vertexData[start + 5].position.x;
			vertexData[start + 5].normal.y = vertexData[start + 5].position.y;
			vertexData[start + 5].normal.z = vertexData[start + 5].position.z;
		}
	}

	materialData_ = std::make_unique<Material>();
	materialData_->Initialize();
	

	// カメラ
	cameraResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(CameraForGPU));
	cameraResorce_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	// ライティング
	lightResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(WritingStyle));
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

	// デフォルト値
	lightData->directionLight_.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	lightData->directionLight_.direction = { 0.0f, -1.0f, 0.0f };
	lightData->directionLight_.intensity = 1.0f;
	lightData->dissolve_.threshold = 0.0f;
	lightData->dissolve_.edgeColor = { 1.0f,0.4f,0.3f };
	lightData->dissolve_.isGradient = false;

	// UVTransform用の行列
	Matrix4x4 uvTransformMatrix = Math::MakeAffineMatrix(Vector3{ 2,2,0 }, Vector3{ 0,0,0 }, Vector3{ 0,0,0 });
	lightData->dissolve_.uvTransform = uvTransformMatrix;
};

void Sphere::SetColor(Vector4 color) {
	materialData_->SetColor(color);
}

void Sphere::SetBlendMode(BlendMode blendMode) {
	if (blendMode_ != blendMode) {
		blendMode_ = blendMode;
		sPipeline();
	}
}

void Sphere::SetShininess(float i) { materialData_->SetShininess(i); }

Sphere* Sphere::CreateSphere(const std::string& texturePath) {
	Sphere* sphere = new Sphere;
	sphere->Initialize(texturePath);
	return sphere;
}

void Sphere::LightDraw(Vector4 color, Vector3 direction, float intensity) {

	lightData->directionLight_.color = color;
	lightData->directionLight_.direction = Math::Normalize(direction);
	lightData->directionLight_.intensity = intensity;
}


void Sphere::LoadTexture(const std::string& texturePath) {
	TextureManager::GetInstance()->Load(texturePath);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(texturePath);
	maskTexture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/Mask/noise0.png");

}


void Sphere::DrawSphere(VertexData* vertexData_, const uint32_t kSubdivision) {
	const float pi = (float)std::numbers::pi;                 // π
	const float kLonEvery = 2.0f * pi / kSubdivision; // 経度分割1つ分の角度(φd)
	const float kLatEvery = pi / kSubdivision;        // 緯度分割1つ分の角度(θd)
	float u;
	float v;
	// 緯度の方向に分割-π/2~π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex; // 現在の緯度(θ)
		// 経度の方向に分割
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery; // 現在の経度(φ)
			u = float(lonIndex) / float(kSubdivision);
			v = 1.0f - float(latIndex) / float(kSubdivision);
			float uv = 1.0f / float(kSubdivision);
			// a 左下
			vertexData_[start].position.x = cos(lat) * cos(lon);
			vertexData_[start].position.y = sin(lat);
			vertexData_[start].position.z = cos(lat) * sin(lon);
			vertexData_[start].position.w = 1.0f;
			vertexData_[start].texcoord = { u, v };
			// b 左上
			vertexData_[start + 1].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexData_[start + 1].position.y = sin(lat + kLatEvery);
			vertexData_[start + 1].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexData_[start + 1].position.w = 1.0f;
			vertexData_[start + 1].texcoord = { u, v - uv };
			// c 右下
			vertexData_[start + 2].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexData_[start + 2].position.y = sin(lat);
			vertexData_[start + 2].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexData_[start + 2].position.w = 1.0f;
			vertexData_[start + 2].texcoord = { u + uv, v };

			// b 左上
			vertexData_[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			vertexData_[start + 3].position.y = sin(lat + kLatEvery);
			vertexData_[start + 3].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			vertexData_[start + 3].position.w = 1.0f;
			vertexData_[start + 3].texcoord = { u + uv, v - uv };
			// d 右上
			vertexData_[start + 4].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexData_[start + 4].position.y = sin(lat);
			vertexData_[start + 4].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexData_[start + 4].position.w = 1.0f;
			vertexData_[start + 4].texcoord = { u + uv, v };
			// c 右下
			vertexData_[start + 5].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexData_[start + 5].position.y = sin(lat + kLatEvery);
			vertexData_[start + 5].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexData_[start + 5].position.w = 1.0f;
			vertexData_[start + 5].texcoord = { u, v - uv };

			vertexData_[start].normal.x = vertexData_[start].position.x;
			vertexData_[start].normal.y = vertexData_[start].position.y;
			vertexData_[start].normal.z = vertexData_[start].position.z;

			vertexData_[start + 1].normal.x = vertexData_[start + 1].position.x;
			vertexData_[start + 1].normal.y = vertexData_[start + 1].position.y;
			vertexData_[start + 1].normal.z = vertexData_[start + 1].position.z;

			vertexData_[start + 2].normal.x = vertexData_[start + 2].position.x;
			vertexData_[start + 2].normal.y = vertexData_[start + 2].position.y;
			vertexData_[start + 2].normal.z = vertexData_[start + 2].position.z;

			vertexData_[start + 3].normal.x = vertexData_[start + 3].position.x;
			vertexData_[start + 3].normal.y = vertexData_[start + 3].position.y;
			vertexData_[start + 3].normal.z = vertexData_[start + 3].position.z;

			vertexData_[start + 4].normal.x = vertexData_[start + 4].position.x;
			vertexData_[start + 4].normal.y = vertexData_[start + 4].position.y;
			vertexData_[start + 4].normal.z = vertexData_[start + 4].position.z;

			vertexData_[start + 5].normal.x = vertexData_[start + 5].position.x;
			vertexData_[start + 5].normal.y = vertexData_[start + 5].position.y;
			vertexData_[start + 5].normal.z = vertexData_[start + 5].position.z;
		}
	}
}


void Sphere::DirectionalLightDraw(DirectionLight directionLight) {
	lightData->directionLight_.color = directionLight.color;
	lightData->directionLight_.direction = Math::Normalize(directionLight.direction);
	lightData->directionLight_.intensity = directionLight.intensity;
	lightData->directionLight_.isEnable_ = true;
}
void Sphere::PointLightDraw(PointLight pointLight, Vector3 direction) {
	lightData->pointLight_ = pointLight;
	lightData->directionLight_.direction = Math::Normalize(direction);
	lightData->pointLight_.isEnable_ = true;
	lightData->directionLight_.intensity = 0.0f;

}
void Sphere::SpotLightDraw(SpotLight spotLight) {
	lightData->spotLight_ = spotLight;
	lightData->spotLight_.direction_ = Math::Normalize(spotLight.direction_);
	lightData->spotLight_.isEnable_ = true;
}

void Sphere::SetMaskTexture(const std::string& path) {
	TextureManager::GetInstance()->Load("resources/Mask/" + path);
	texture_ = TextureManager::GetInstance()->GetTextureIndexByFilePath("resources/Mask/" + path);
}