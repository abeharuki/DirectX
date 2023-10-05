#include "Model.h"
#include <cassert>
#include <format>

#include "StringUtility.h"

// コマンドリスト
ID3D12GraphicsCommandList* Model::sCommandList_ = nullptr;
// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::rootSignature_;
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> Model::sPipelineState_;
Microsoft::WRL::ComPtr<ID3DBlob> Model::signatureBlob_;
Microsoft::WRL::ComPtr<ID3DBlob> Model::errorBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> Model::vertexShaderBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> Model::pixelShaderBlob_;
D3D12_VIEWPORT Model::viewport{};
D3D12_RECT Model::scissorRect{};
// 頂点バッファビュー
D3D12_VERTEX_BUFFER_VIEW Model::vbView_ = {};
D3D12_INDEX_BUFFER_VIEW Model::ibView_ = {};
// 頂点
Microsoft::WRL::ComPtr<ID3D12Resource> Model::vertexResource_;
// ライティング
Microsoft::WRL::ComPtr<ID3D12Resource> Model::lightResource_;
// WVP用リソース
Microsoft::WRL::ComPtr<ID3D12Resource> Model::wvpResouce_;
// マテリアル用リソース
Microsoft::WRL::ComPtr<ID3D12Resource> Model::materialResorce_;
TransformationMatrix* Model::wvpData;
Transform Model::transform = {
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f}
};
Transform Model::cameraTransform = {
    {1.0f, 1.0f, 1.0f  },
    {0.0f, 0.0f, 0.0f  },
    {0.0f, 0.0f, -10.0f}
};
D3D12_GPU_DESCRIPTOR_HANDLE Model::textureSrvHandleGPU;

IDxcBlob* Model::CompileShader(
    // CompilerするShaderファイルへのパス
    const std::wstring& filePath,
    // Compilerに使用するProfile
    const wchar_t* profile,
    // 初期化で生成したものを3つ
    IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
	// これからシェーダーをコンパイルする旨をログに出す
	utility_->Log(utility_->ConvertString(
	    std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[] = {
	    filePath.c_str(), // コンパイル対象のhlslファイル名
	    L"-E",
	    L"main", // エントリーポイントの指定。基本的にmain以外にはしない
	    L"-T",
	    profile, // ShaderProfileの設定
	    L"-Zi",
	    L"-Qembed_debug", // デバッグ用の情報を埋め込む
	    L"-Od",           // 最適化を外しておく
	    L"-Zpr",          // メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
	    &shaderSourceBuffer,        // 読み込んだファイル
	    arguments,                  // コンパイルオプション
	    _countof(arguments),        // コンパイルオプションの数
	    includeHandler,             // includeが含まれた諸々
	    IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		utility_->Log(shaderError->GetStringPointer());
		// 警告・エラーダメ絶対
		assert(false);
	}

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	utility_->Log(utility_->ConvertString(
	    std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}


//Textureデータの読み込み
DirectX::ScratchImage Model::LoadTexture(const std::string& filePath) {
	// テクスチャファイルを読み込んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = utility_->ConvertString(filePath);
	HRESULT hr =
	    DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
	    image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0,
	    mipImages);

	// ミニマップ付きのデータを返す
	return mipImages;
};

//TextureResourceの作成
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
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

//TextureResourceにデータ転送
void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
		    UINT(mipLevel),
		    nullptr,              // 全領域へコピー
		    img->pixels,          // 元データアドレ
		    UINT(img->rowPitch),  // 1ランサイズ
		    UINT(img->slicePitch) // 1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}


ID3D12DescriptorHeap* CreateDescriptorHeap(
    ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors,
    bool shaderVisible) {
	// ディスクリプタヒープの生成
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;                 // レンダーターゲットビュー用
	descriptorHeapDesc.NumDescriptors = numDescriptors; // ダブルバッファ用に二つ。多くても可
	descriptorHeapDesc.Flags =
	    shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));

	return descriptorHeap;
};


Model* Model::GetInstance() {
	static Model instance;
	return &instance;

}

void Model::Initializ(DirectXCommon* dxCommon) { 
	dxCommon_ = dxCommon;
	mesh_ = new Mesh;
	InitializeGraphicsPipeline();
}


/// <summary>
/// グラフィックスパイプラインの初期化
/// </summary>
void Model::InitializeGraphicsPipeline(){

	HRESULT hr_ = S_FALSE;

	// dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr_));
	hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr_));

	// 現時点でincludeしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler = nullptr;
	hr_ = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr_));


	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
	// RootSignature作成. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	rootParameters[2].ParameterType =
	    D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;                     // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges =
	    _countof(descriptorRange); // Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ


	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;   // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけMipmapを使う
	staticSamplers[0].ShaderRegister = 0;         // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	hr_ = D3D12SerializeRootSignature(
	    &descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_,&errorBlob_);
	if (FAILED(hr_)) {
		utility_->Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	
	// バイナリを元に生成
	hr_ = dxCommon_->GetDevice()->CreateRootSignature(
	    0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
	    IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr_));

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.AlphaToCoverageEnable = FALSE;//アンチエイリアシング有無
	blendDesc.IndependentBlendEnable = FALSE;//ブレンドステートを個別化するか有無
	blendDesc.RenderTarget[0].BlendEnable = TRUE;//ブレンディング有無
	//ブレンディング係数の設定
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	

	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	// Shaderをコンパイルする
	vertexShaderBlob_ =
	    CompileShader(L"Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ =
	    CompileShader(L"Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob_ != nullptr);


	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	

	// PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get(); // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;  // InputLayout
	graphicsPipelineStateDesc.VS = {
	    vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()}; // VertexShader
	graphicsPipelineStateDesc.PS = {
	    pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize()}; // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;              // BrendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;                 // RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトロポジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	hr_ = dxCommon_->GetDevice()->CreateGraphicsPipelineState(
	    &graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineState_));
	assert(SUCCEEDED(hr_));


	mesh_->CreateBuffers(dxCommon_->GetDevice());

	

	//バーテックス
	vbView_ = mesh_->GetVBView();
	vertexResource_ = mesh_->GetVertex();
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	// 左下
	vertexData[0].position = {-0.5f, -0.5f, 0.0f, 1.0f}; // 左下
	vertexData[0].texcoord = {0.0f, 1.0f};
	// 上
	vertexData[1].position = {0.0f, 0.5f, 0.0f, 1.0f}; // 左上
	vertexData[1].texcoord = {0.5f, 0.0f};
	// 右下
	vertexData[2].position = {0.5f, -0.5f, 0.0f, 1.0f}; // 右下
	vertexData[2].texcoord = {1.0f, 1.0f};

	vertexData[3].position = {-0.5f, -0.5f, 0.5f, 1.0f}; // 左上
	vertexData[3].texcoord = {0.0f, 1.0f};
	
	vertexData[4].position = {0.0f, 0.0f, 0.0f, 1.0f}; // 上
	vertexData[4].texcoord = {0.5f, 0.0f};

	vertexData[5].position = {0.5f, -0.5f, -0.5f, 1.0f}; // 右下
	vertexData[5].texcoord = {1.0f, 1.0f};

	//インデックス
	ibView_ = mesh_->GetIBView();
	

	//WVP
	wvpResouce_ = mesh_->GetWVP();
	// データを書き込む
	// 書き込むためのアドレスを取得
	wvpResouce_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込む
	wvpData->WVP = Math::MakeIdentity4x4();
	wvpData->World = Math::MakeIdentity4x4();


	//マテリアル
	materialResorce_ = mesh_->GetMaterial();

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
	


	//ライティング
	lightResource_ = mesh_->GetLight();
	// 頂点リソースにデータを書き込む
	// 書き込むためのアドレスを取得
	lightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// デフォルト値
	directionalLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	directionalLightData->direction = {0.0f, -1.0f, 0.0f};
	directionalLightData->intensity = 1.0f;



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

	LoadTexture();

	
};


void Model::PreDraw(ID3D12GraphicsCommandList* commandList) { 
	sCommandList_ = commandList;

	// ゲームの処理
	transform.rotate.y += 0.03f;
	Matrix4x4 worldMatrix =
	    Math::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = Math::MakeAffineMatrix(
	     cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Math::Inverse(cameraMatrix);
	Matrix4x4 projecttionMatrix =
	    Math::MakePerspectiverFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	// WVPMatrixを作る。同次クリップ空間
	Matrix4x4 worldViewProjectionMatrix =
	    Math::Multiply(worldMatrix, Math::Multiply(viewMatrix, projecttionMatrix));
	wvpData->WVP = worldViewProjectionMatrix;
	

	// コマンドを積む
	sCommandList_->RSSetViewports(1, &viewport);
	sCommandList_->RSSetScissorRects(1, &scissorRect);
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	sCommandList_->SetGraphicsRootSignature(rootSignature_.Get());
	sCommandList_->SetPipelineState(sPipelineState_.Get());



	//sCommandList_->IASetIndexBuffer(&ibView_); // IBVを設定
	//sCommandList_->DrawIndexedInstanced(6, 1, 0, 0, 0);

	
	
	
}

void Model::PostDraw() {
	// コマンドリストを解除
	Model::sCommandList_ = nullptr;
}

void Model::Draw() {

	sCommandList_->IASetVertexBuffers(0, 1, &vbView_);
	// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(0, materialResorce_->GetGPUVirtualAddress());
	sCommandList_->SetGraphicsRootConstantBufferView(3, lightResource_->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭の設定。2はrootParameter[2]である
	sCommandList_->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
	// wvp用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(1, wvpResouce_->GetGPUVirtualAddress());

	// 三角形の描画
	sCommandList_->DrawInstanced(6, 1, 0, 0);
}

//画像の読み込み
void Model::LoadTexture() {

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	ID3D12Resource* textureResource = CreateTextureResource(dxCommon_->GetDevice(), metadata);
	UploadTextureData(textureResource, mipImages);

	// metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU =
	    dxCommon_->GetSRV()->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU =
	    dxCommon_->GetSRV()->GetGPUDescriptorHandleForHeapStart();

	// 先頭はImGuiが使っているのでその次を使う
	textureSrvHandleCPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(
	    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(
	    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// SRVの作成
	dxCommon_->GetDevice()->CreateShaderResourceView(
	    textureResource, &srvDesc, textureSrvHandleCPU);

	
}