#include "Model.h"
#include <cassert>
#include <format>

#include "StringUtility.h"

#include <imgui.h>


Microsoft::WRL::ComPtr<ID3D12Resource> Model::lightResource_;
DirectionalLight* Model::directionalLightData;

void Model::Initialize(const std::string& filename, const std::string& texturePath) { 
	
	LoadTexture(filename, texturePath);
	CreateVertexResource();
	sPipeline();
	
	
}



void Model::sPipeline() {
	
	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreateVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePSShader();
	
	
	rootSignature_ = GraphicsPipeline::GetInstance()->CreateRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreateGraphicsPipeline(blendMode_);

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


void Model::Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection ,bool light) {
	wvpResouce_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(TransformationMatrix));
	// データを書き込む
	wvpData = nullptr;
	// 書き込むためのアドレスを取得
	wvpResouce_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = Math::MakeIdentity4x4();
	wvpData->World = Math::MakeIdentity4x4();
	
	Matrix4x4 worldViewProjectionMatrixSprite = Math::Multiply(
	    worldTransform.matWorld_,
	    Math::Multiply(viewProjection.matView, viewProjection.matProjection));
	*wvpData = TransformationMatrix(worldViewProjectionMatrixSprite, worldTransform.matWorld_);

	//ライティング有効化
	materialData->enableLighting = light;

	//  コマンドを積む
	Engine::GetList()->RSSetViewports(1, &viewport);
	Engine::GetList()->RSSetScissorRects(1, &scissorRect);

	

	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
	Engine::GetList()->SetPipelineState(sPipelineState_.Get());

	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	
	
	//


	Engine::GetList()->SetDescriptorHeaps(1, Engine::GetSRV().GetAddressOf());
	Engine::GetList()->SetGraphicsRootDescriptorTable(2, textureManager_->GetGPUHandle(texture_));

    // wvp用のCBufferの場所を設定
	// マテリアルCBufferの場所を設定
	Engine::GetList()->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(1, wvpResouce_->GetGPUVirtualAddress());
	Engine::GetList()->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());

	// 三角形の描画
	Engine::GetList()->DrawInstanced(UINT(modelData.vertices.size()), instanceCount, 0, 0);

	
}

//頂点データの設定
void Model::CreateVertexResource() {
	// モデルの読み込み 
	// 頂点リソースを作る
	vertexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) * modelData.vertices.size());
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得

	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation =vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(
	    sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

	
	std::memcpy( vertexData, modelData.vertices.data(),sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースにコピース
	
	

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

	
};

void Model::SetColor(Vector4 color) {
	materialData->color.rgb = {color.x, color.y, color.z};
	materialData->color.a = color.w;
}

void Model::SetBlendMode(BlendMode blendMode) { 
	blendMode_ = blendMode; 

}

Model* Model::CreateModelFromObj(const std::string& filename, const std::string& texturePath) {
	Model* model = new Model;
	model->Initialize(filename, texturePath);
	return model;


}

void Model::LoadTexture(const std::string& filename, const std::string& texturePath) {
	modelData = TextureManager::LoadObjFile(filename);
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();
	texture_ = textureManager_->Load(texturePath);
	
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& filename) {
	// 宣言
	MaterialData materialData; // 構築するMaterialData
	ModelData modelData;
	std::string line;                              // ファイルから読んだ1行を格納する
	std::ifstream file("./" +filename); // ファイルを開く
	assert(file.is_open());                        // 開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結しているファイルパス
			materialData.textureFilePath = textureFilename;
		} 
	}

	return materialData;
}

ModelData Model::LoadObjFile(const std::string& filename) {
	// 必要な変数の宣言
	ModelData modelData;            // 構築するModelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals;   // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line;               // ファイルから選んだ１行を格納する

	// ファイルを開く
	std::ifstream file(filename);
	assert(file.is_open()); // とりあえず開けなかったら止める

	// ファイルを読み,ModelDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identfierの応じた処理
		// 頂点情報を読み込む
		if (identifier == "v") {
			Vector4 position = {0.0f, 0.0f, 0.0f, 0.0f};
			s >> position.x >> position.y >> position.z;
			position.z *= -1.0f; // 位置の反転
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord = {0.0f, 0.0f};
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal = {0.0f, 0.0f, 0.0f};
			normal.z *= -1.0f; // 法線の反転
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形減退。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは[位置/UV/法線]で格納しているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // 区切りでインデックスを読んでく
					elementIndices[element] = std::stoi(index);
				}

				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				// VertexData vertex = { position,texcoord,normal };
				// modelData.vertices.push_back(vertex);
				triangle[faceVertex] = {position, texcoord, normal};
			}

			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		} else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得
			std::string materialFilname;
			s >> materialFilname;
			// 基本的にobjファイルと同一階層にmtlは存在させるから、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(materialFilname);
		}
	}
	return modelData;
}

void Model::LightDraw(Vector4 color, Vector3 direction, float intensity) {
	
	directionalLightData->color = color;
	directionalLightData->direction = Math::Normalize(direction);
	directionalLightData->intensity = intensity;
	
}

void Model::PostDraw() {}


