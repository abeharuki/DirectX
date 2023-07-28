#include <Windows.h>
#include <cstdint>
#include <string>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lPaeam);
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include <vector>
#include <fstream>
#include <sstream>
#include "ResourceObject.h"
#include <wrl.h>
#include "Math.h"

#include "WinApp.h"
#include "DirectXCommon.h"
#include "Model.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxcompiler.lib")


struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Sphere
{
	Vector3 center;//中心点
	float radius;//半径

};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct MaterialData {

	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData>vertices;
	MaterialData material;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;

};

struct DirectionalLight
{
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度

};

ID3D12Resource*
    CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;                          // Textureの幅
	resourceDesc.Height = height;                        // Textureの高さ
	resourceDesc.MipLevels = 1;                          // mipmapの数
	resourceDesc.DepthOrArraySize = 1;                   // 奥行き or 配列Textureの配列
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;              // 1.0f(最大値)クリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる

	// Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
	    &heapProperties,                  // Heapの設定
	    D3D12_HEAP_FLAG_NONE,             // Heapの特殊な設定。特になし。
	    &resourceDesc,                    // Resourceの設定
	    D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
	    &depthClearValue,                 // Clear最適値
	    IID_PPV_ARGS(&resource));         // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

ID3D12Resource* CreateTextureResource(
    ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);                  // Textureの幅
	resourceDesc.Height = UINT(metadata.height);                // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);        // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;                      // TextureのFormat
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
	resourceDesc.Dimension =
	    D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使ってるのは2次元

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
	heapProperties.CPUPageProperty =
	    D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

	// Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
	    &heapProperties,                   // Heapの設定
	    D3D12_HEAP_FLAG_NONE,              // Heapの特殊な設定。特になし。
	    &resourceDesc,                     // Resourceの設定
	    D3D12_RESOURCE_STATE_GENERIC_READ, // 初回のResourceState。Textureは基本読むだけ
	    nullptr,
	    IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}




/*






DirectX::ScratchImage LoadTexture(const std::string& filePath) {
	//テクスチャファイルを読み込んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);

	//ミニマップ付きのデータを返す
	return mipImages;
};




D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	//Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		//MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		//Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,//全領域へコピー
			img->pixels,//元データアドレ
			UINT(img->rowPitch),//1ランサイズ
			UINT(img->slicePitch)//1枚サイズ
			);
		assert(SUCCEEDED(hr));
	}

}

void DrawSphere(VertexData* vertexData) {
	const uint32_t kSubdivision = 10;//分割数
	const float pi = 3.14f;//π
	const float kLonEvery = 2.0f * pi / kSubdivision;//経度分割1つ分の角度(φd)
	const float kLatEvery = pi / kSubdivision;//緯度分割1つ分の角度(θd)
	float u; 
	float v;
	//緯度の方向に分割-π/2~π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//現在の緯度(θ)
		//経度の方向に分割
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery;//現在の経度(φ)
			u = float(lonIndex) / float(kSubdivision);
			v = 1.0f - float(latIndex) / float(kSubdivision);
			//a 左下
			vertexData[start].position.x = cos(lat) * cos(lon);
			vertexData[start].position.y = sin(lat);
			vertexData[start].position.z = cos(lat) * sin(lon);
			vertexData[start].position.w = 1.0f;
			vertexData[start].texcoord = { u , v + 0.1f };
			//b 左上
			vertexData[start+1].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexData[start+1].position.y = sin(lat + kLatEvery);
			vertexData[start+1].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexData[start+1].position.w = 1.0f;
			vertexData[start+1].texcoord = { u, v };
			//c 右下
			vertexData[start+2].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexData[start+2].position.y = sin(lat);
			vertexData[start+2].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexData[start+2].position.w = 1.0f;
			vertexData[start+2].texcoord = { u + 0.1f, v + 0.1f };


			// d 右上
			vertexData[start + 3].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			vertexData[start + 3].position.y = sin(lat + kLatEvery);
			vertexData[start + 3].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			vertexData[start + 3].position.w = 1.0f;
			vertexData[start + 3].texcoord = {u + 0.1f, v};
			
			//b 左上
			vertexData[start+3].position.x = cos(lat + kLatEvery) * cos(lon);
			vertexData[start+3].position.y = sin(lat + kLatEvery);
			vertexData[start+3].position.z = cos(lat + kLatEvery) * sin(lon);
			vertexData[start+3].position.w = 1.0f;
			vertexData[start+3].texcoord = { u, v };
			//d 右上
			vertexData[start+4].position.x = cos(lat + kLatEvery) * cos(lon + kLonEvery);
			vertexData[start+4].position.y = sin(lat + kLatEvery);
			vertexData[start+4].position.z = cos(lat + kLatEvery) * sin(lon + kLonEvery);
			vertexData[start+4].position.w = 1.0f;
			vertexData[start+4].texcoord =  { u + 0.1f, v  };
			//c 右下
			vertexData[start+5].position.x = cos(lat) * cos(lon + kLonEvery);
			vertexData[start+5].position.y = sin(lat);
			vertexData[start+5].position.z = cos(lat) * sin(lon + kLonEvery);
			vertexData[start+5].position.w = 1.0f;
			vertexData[start+5].texcoord =  { u + 0.1f, v + 0.1f };
			
			vertexData[start].normal.x = vertexData[start].position.x;
			vertexData[start].normal.y = vertexData[start].position.y;
			vertexData[start].normal.z = vertexData[start].position.z;

			vertexData[start+1].normal.x = vertexData[start+1].position.x;
			vertexData[start+1].normal.y = vertexData[start+1].position.y;
			vertexData[start+1].normal.z = vertexData[start+1].position.z;

			vertexData[start+2].normal.x = vertexData[start+2].position.x;
			vertexData[start+2].normal.y = vertexData[start+2].position.y;
			vertexData[start+2].normal.z = vertexData[start+2].position.z;

			vertexData[start+3].normal.x = vertexData[start+3].position.x;
			vertexData[start+3].normal.y = vertexData[start+3].position.y;
			vertexData[start+3].normal.z = vertexData[start+3].position.z;
			
			vertexData[start+4].normal.x = vertexData[start+4].position.x;
			vertexData[start+4].normal.y = vertexData[start+4].position.y;
			vertexData[start+4].normal.z = vertexData[start+4].position.z;

			vertexData[start+5].normal.x = vertexData[start+5].position.x;
			vertexData[start+5].normal.y = vertexData[start+5].position.y;
			vertexData[start+5].normal.z = vertexData[start+5].position.z;
			
		}
		
	}
	
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	//必要な変数の宣言
	ModelData modelData;//構築するModelData
	std::vector<Vector4>positions; //位置
	std::vector<Vector3>normals;//法線
	std::vector<Vector2>texcoords;//テクスチャ座標
	std::string line;//ファイルから選んだ１行を格納する

	//ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//とりあえず開けなかったら止める

	//ファイルを読み,ModelDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		//identfierの応じた処理
		//頂点情報を読み込む
		if (identifier == "v") {
			Vector4  position = { 0.0f,0.0f,0.0f,0.0f };
			s >> position.x >> position.y >> position.z;
			position.z *= -1.0f; // 位置の反転
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord = {0.0f, 0.0f};
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal = {0.0f,0.0f,0.0f};
			normal.z *= -1.0f;//法線の反転
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (identifier =="f") {
			VertexData triangle[3];
		 //面は三角形減退。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは[位置/UV/法線]で格納しているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');//区切りでインデックスを読んでく	
					elementIndices[element] = std::stoi(index);
				}


				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position,texcoord,normal };
				//modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}

			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		}




	}
	return modelData;
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//宣言
	MaterialData materialData;//構築するMaterialData
	ModelData modelData;
	std::string line;//ファイルから読んだ1行を格納する
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結しているファイルパス
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;

			//基本的にobjファイルと同じ一階層にmtlは存在させるので、ディレクション名などファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);

		}

	}

	return materialData;
}
*/

// WIndowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	Model* model = nullptr;
	
	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win,1280,720);

	model = Model::GetInstance();
	model->Initializ(dxCommon);
	

	/*

	ID3D12Device* device = nullptr;

	
	
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	

	//RTV用のヒープでディスクリプタの数は2。RTVはShader内で触れるものではない
	ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//SRV用のヒープでディスクリプタの数は128。SRVはShader内で触るものではないので、ShaderVisibleはtrue
	ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);


	
	

	//球
	ID3D12Resource* vertexResource = CreateBufferResoure(device, sizeof(VertexData) * 1536);
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 1536;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexData));

	DrawSphere(vertexData);
	

	// インデックスを使った球
	// インデックス用の頂点リソースを作る
	ID3D12Resource* indexResource = CreateBufferResoure(device, sizeof(uint32_t) * 1536);
	// 頂点バッファビューを作成する
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	// リソースの先頭アドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 1536;
	// インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	// データを書き込む
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));


	/*
	/モデルの読み込み
	ModelData modelData = LoadObjFile("resources", "axis.obj");
	modelData.material = LoadMaterialTemplateFile("resources", "plane.mtl");
	//頂点リソースを作る
	ID3D12Resource* vertexResource = CreateBufferResoure(device, sizeof(VertexData) * modelData.vertices.size());
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();//リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());//使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);//1頂点あたりのサイズ

	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());//頂点データをリソースにコピー
	*/

	
    /*
	//Sprite用の頂点リソースを作る
	ID3D12Resource* vertexResourceSprite = CreateBufferResoure(device, sizeof(VertexData) * 4);
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, 
		reinterpret_cast<void**>(&vertexDataSprite));

	//1枚目の三角形
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//左下
	vertexDataSprite[0].texcoord = { 0.0f,1.0f };
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//左上
	vertexDataSprite[1].texcoord = { 0.0f,0.0f };
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//右下
	vertexDataSprite[2].texcoord = { 1.0f,1.0f };
	vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//右上
	vertexDataSprite[3].texcoord = { 1.0f,0.0f };
	
	
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };
	
	
	//インデックスを使った四角形
	//インデックス用の頂点リソースを作る
	ID3D12Resource* indexResourceSprite = CreateBufferResoure(device, sizeof(uint32_t) * 6);
	// 頂点バッファビューを作成する
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	//リソースの先頭アドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
	//データを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;

	

	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ID3D12Resource* wvpResouce = CreateBufferResoure(device, sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResouce->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込む
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();
	

	//Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズえお用意する
	ID3D12Resource* transformationMatrixResourceSprite = CreateBufferResoure(device, sizeof(TransformationMatrix));
	//データを書き込む
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//単位行列に書き込んでいく
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
	transformationMatrixDataSprite->World = MakeIdentity4x4();

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	ID3D12Resource* materialResorce = CreateBufferResoure(device, sizeof(Material));
	//マテリアルにデータを書き込む
	Material* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResorce->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//今回は白を書き込む
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//Lightingを有効にする
	materialData->enableLighting = true;
	//初期化
	materialData->uvTransform = MakeIdentity4x4();
	
	//Sprite用のマテリアルリソースを作る
	ID3D12Resource* materialResourceSprite = CreateBufferResoure(device, sizeof(Material));
	//データを書き込む
	Material* materialDataSprite = nullptr;
	//書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	//今回は白を書き込む
	materialDataSprite->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//Lightingを無効
	materialDataSprite->enableLighting = false;
	//初期化
	materialDataSprite->uvTransform = MakeIdentity4x4();
	
	ID3D12Resource* directionalLightResource = CreateBufferResoure(device, sizeof(DirectionalLight));
	// 頂点リソースにデータを書き込む
	DirectionalLight* directionalLightData = nullptr;
	// 書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr,reinterpret_cast<void**>(&directionalLightData));

	//デフォルト値
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;
	
	

	
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(device, metadata);
	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);
	UploadTextureData(textureResource, mipImages);

	// 2枚目のTextrueを読んで転送する
	DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ID3D12Resource* textureResource2 = CreateTextureResource(device, metadata2);
	UploadTextureData(textureResource2, mipImages2);

	//objファイル
	/*
	/2枚目のTextrueを読んで転送する
	DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	ID3D12Resource* textureResource2 = CreateTextureResource(device, metadata2);
	UploadTextureData(textureResource2, mipImages2);
	*/
	/*
	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	//先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVの作成
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);


	//2つ目のmetadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	const uint32_t desriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	const uint32_t desriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	const uint32_t desriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//2つ目のSRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap, desriptorSizeSRV, 2);
	//SRVの作成
	device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);

	

	

	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDdesc{};
	dsvDdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResourceに合わせる
	dsvDdesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	//DSVHeapの先頭にDSVを作る
	device->CreateDepthStencilView(depthStencilResource, &dsvDdesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//ImGuiの初期化。
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(device,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap,
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	ResourceObject deptStencilResource =
	    CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);
	bool useMonsterBall = true;
	*/

	MSG msg{};
	// ウインドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		
		// Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			

			/*
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			Math* math_;
			
			// ゲームの処理
			//transform.rotate.y += 0.03f;
			Matrix4x4 worldMatrix = math_->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = math_->MakeAffineMatrix(
			    cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = math_->Inverse(cameraMatrix);
			Matrix4x4 projecttionMatrix =
			    math_->MakePerspectiverFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
			//WVPMatrixを作る。同次クリップ空間
			Matrix4x4 worldViewProjectionMatrix =
			    math_->Multiply(worldMatrix, math_->Multiply(viewMatrix, projecttionMatrix));
			wvpData->WVP = worldViewProjectionMatrix;

			
			//Sprite用のworldViewProjectionMatrixを作る
			Matrix4x4 worldMatrixSprite = math_->MakeAffineMatrix(
			    transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = math_->MakeOrthographicMatrix(
			    0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
			Matrix4x4 worldViewProjectionMatrixSprite = math_->Multiply(
			    worldMatrixSprite, math_->Multiply(viewMatrixSprite, projectionMatrixSprite));
			*transformationMatrixDataSprite = TransformationMatrix( worldViewProjectionMatrixSprite, worldMatrixSprite);
			
			//UVTransform用の行列
			Matrix4x4 uvTransformMatrix = math_->MakeAffineMatrix(
			    uvTransformSprite.scale, uvTransformSprite.rotate, uvTransformSprite.translate);

			materialDataSprite->uvTransform = uvTransformMatrix;

			
		

		
			
			//マテリアルCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResorce->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
			
			//SRVのDescriptorTableの先頭の設定。2はrootParameter[2]である
			commandList->SetGraphicsRootDescriptorTable(2,useMonsterBall ? textureSrvHandleGPU2 :  textureSrvHandleGPU);
			//wvp用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(1, wvpResouce->GetGPUVirtualAddress());
			commandList->IASetIndexBuffer(&indexBufferView);//IBVを設定
			//球の描画
			commandList->DrawInstanced(1536, 1, 0, 0);
			//commandList->DrawIndexedInstanced(1536, 1, 0, 0, 0);
			//commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

			//CBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			
			//TransformationMatrixCBufferの場所を設定
			commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			//Spriteをインデックス描画。
			commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);//VBVを設定
			commandList->IASetIndexBuffer(&indexBufferViewSprite);//IBVを設定
			//描画
			//commandList->DrawInstanced(6, 1, 0, 0);
			commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
			

			ImGui::Begin("Settings");
			ImGui::SliderFloat3("color", &materialData->color.x, 0.0f, 1.0f);
			//Sprite
			ImGui::SliderFloat3("translationSprite", &transformSprite.translate.x, 0.0f, 1280.0f);
			ImGui::Checkbox("useMonsterBall", &useMonsterBall);
			//LightLight
			ImGui::SliderFloat3("LightColor", &directionalLightData->color.x, -1.0f,1.0f);
			ImGui::SliderFloat3("LightDirecton", &directionalLightData->direction.x, -1.0f,1.0f);
			ImGui::DragFloat("Intensity", &directionalLightData->intensity, 0.1f);
			//UV
			ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f,10.0f);
			ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f,-10.0f,10.0f);
			ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);


			ImGui::DragFloat3("Sphere.Rotate", &transform.rotate.x, 0.01f, -10.0f, 10.0f);
			ImGui::End();

			//開発用UIの処理。
			ImGui::ShowDemoWindow();


			//ImGuiの内部コマンドを生成する 
			ImGui::Render();

			// 画面に描く処理はすべて終わり、画面に映すので、状態を遷移
			//実際のcommandListのImGuiの描画コマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

			
			



			*/
			//描画開始
			dxCommon->PreDraw();

			model->PreDraw();

			//描画終了
			dxCommon->PostDraw();
            
		}
		
	}
	
	/*
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	
	//解放処理
	ResourceObject deptStencilResource =
	    CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);
	CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
	
#ifdef _DEBUG
	debugController->Release();
#endif
	CloseWindow(hwnd);
	
	srvDescriptorHeap->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();
	materialResorce->Release();
	wvpResouce->Release();
	materialResourceSprite->Release();
	directionalLightResource->Release();
	transformationMatrixResourceSprite->Release();
	vertexResource->Release();
	vertexResourceSprite->Release();
	indexResourceSprite->Release();
	textureResource->Release();
	depthStencilResource->Release();
	dsvDescriptorHeap->Release();
	textureResource2->Release();
	indexResource->Release();

	
	}
	*/
	
	//リソースリークチェック
	//dxCommon->Debug();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}



