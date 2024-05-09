#include "Line.h"
#include <cassert>
#include <format>
#include <imgui.h>
#include "Model.h"
#include <numbers> 

Microsoft::WRL::ComPtr<IDxcBlob> Line::vertexShaderBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> Line::pixelShaderBlob_;

Microsoft::WRL::ComPtr<ID3D12Resource> Line::lightResource_;
WritingStyle* Line::lightData;


void Line::Initialize(Vector3 start, Vector3 end) {
	CreateVertexResource(start, end);
	sPipeline();
}


void Line::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreateLinePSShader();

	rootSignature_ = GraphicsPipeline::GetInstance()->CreateLineRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateLineGraphicsPipeline(blendMode_);


};



void Line::Draw(
	WorldTransform& worldTransform, const ViewProjection& viewProjection, bool light) {
	cameraData->worldPos = viewProjection.translation_;


	// ライティング有効化
	materialData->enableLighting = light;



	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());

	// wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(3, viewProjection.constBuff_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(4, cameraResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(2, lightResource_->GetGPUVirtualAddress());

	// 三角形の描画
	Engine::GetList()->DrawInstanced(2, 1, 0, 0);
}

// 頂点データの設定
void Line::CreateVertexResource(Vector3 start, Vector3 end) {

	// モデルの読み込み
	// 頂点リソースを作る
	vertexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) * 2);
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得

	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * 2); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	vertexData[0].position.x = start.x;
	vertexData[0].position.y = start.y;
	vertexData[0].position.z = start.z;
	vertexData[0].position.w = 1.0f;

	vertexData[1].position.x = end.x;
	vertexData[1].position.y = end.y;
	vertexData[1].position.z = end.z;
	vertexData[1].position.w = 1.0f;


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
	// 光沢
	materialData->shininess = 10.0f;
	// 初期化
	materialData->uvTransform = Math::MakeIdentity4x4();

	// カメラ
	cameraResorce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(CameraForGPU));
	cameraResorce_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	cameraData->worldPos = { 0.0f, 0.0f, -10.0f };

	// ライティング
	lightResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(WritingStyle));
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&lightData));

	// デフォルト値
	lightData->directionLight_.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	lightData->directionLight_.direction = { 0.0f, -1.0f, 0.0f };
	lightData->directionLight_.intensity = 1.0f;
};


void Line::SetLinePos(Vector3 start, Vector3 end) {
	vertexData[0].position.x = start.x;
	vertexData[0].position.y = start.y;
	vertexData[0].position.z = start.z;

	vertexData[1].position.x = end.x;
	vertexData[1].position.y = end.y;
	vertexData[1].position.z = end.z;
}

void Line::SetColor(Vector4 color) {
	materialData->color.rgb = { color.x, color.y, color.z };
	materialData->color.a = color.w;
}

void Line::SetBlendMode(BlendMode blendMode) { blendMode_ = blendMode; }

void Line::SetShininess(float i) { materialData->shininess = i; }

Line* Line::CreateLine(Vector3 start, Vector3 end) {
	Line* line = new Line;
	line->Initialize(start, end);
	return line;
}

void Line::LightDraw(Vector4 color, Vector3 direction, float intensity) {

	lightData->directionLight_.color = color;
	lightData->directionLight_.direction = Math::Normalize(direction);
	lightData->directionLight_.intensity = intensity;
}


void Line::DirectionalLightDraw(DirectionLight directionLight) {
	lightData->directionLight_.color = directionLight.color;
	lightData->directionLight_.direction = Math::Normalize(directionLight.direction);
	lightData->directionLight_.intensity = directionLight.intensity;
	lightData->directionLight_.isEnable_ = true;
	lightData->pointLight_.isEnable_ = false;
	lightData->spotLight_.isEnable_ = false;



}
void Line::PointLightDraw(PointLight pointLight, Vector3 direction) {
	lightData->pointLight_ = pointLight;
	lightData->directionLight_.direction = Math::Normalize(direction);
	lightData->pointLight_.isEnable_ = true;
	lightData->spotLight_.isEnable_ = false;
	lightData->directionLight_.isEnable_ = false;
	lightData->directionLight_.intensity = 0.0f;

}
void Line::SpotLightDraw(SpotLight spotLight) {
	lightData->spotLight_ = spotLight;
	lightData->spotLight_.direction_ = Math::Normalize(spotLight.direction_);
	lightData->spotLight_.isEnable_ = true;
	lightData->pointLight_.isEnable_ = false;
	lightData->directionLight_.isEnable_ = false;

}