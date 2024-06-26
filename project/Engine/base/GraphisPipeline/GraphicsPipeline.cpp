#include "GraphicsPipeline.h"
#include "Model.h"
#include "Engine.h"



GraphicsPipeline* GraphicsPipeline::GetInstance() {
	static GraphicsPipeline Instance;
	return &Instance;
}



Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreateGraphicsPipeline(BlendMode blendMode_) {
	if (sPipelineState_) {
		return sPipelineState_;
	}
	else {
		sPipelineState_ = nullptr;

#pragma region InputLayout

		// InputLayoutの設定
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

#pragma endregion

#pragma region BlendState
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		// すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = FALSE; // アンチエイリアシング有無
		blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;



		// ブレンディング係数の設定
		switch (blendMode_) {
		case BlendMode::kNone:
			blendDesc.RenderTarget[0].BlendEnable = FALSE;
			break;
		case BlendMode::kNormal:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case BlendMode::kAdd:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kSubtract:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kMultily:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		}


#pragma endregion

#pragma region RasiterzerState

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region PSO

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get(); // RootSignature

		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;         // InputLayout

		graphicsPipelineStateDesc.VS = {
			vertexShaderBlob_->GetBufferPointer(),
			vertexShaderBlob_->GetBufferSize() }; // VertexShader
		graphicsPipelineStateDesc.PS = {
			pixelShaderBlob_->GetBufferPointer(),
			pixelShaderBlob_->GetBufferSize() }; // PixelShader

		graphicsPipelineStateDesc.BlendState = blendDesc;           // BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		// 書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

#pragma endregion

		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateGraphicsPipelineState(
			&graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineState_));
		assert(SUCCEEDED(hr_));

		return sPipelineState_;
	}
}

Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreateSkyboxGraphicsPipeline(BlendMode blendMode_) {
	if (skyboxPipelineState_) {
		return skyboxPipelineState_;
	}
	else {
		skyboxPipelineState_ = nullptr;

#pragma region InputLayout

		// InputLayoutの設定
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

#pragma endregion

#pragma region BlendState
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		// すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = FALSE; // アンチエイリアシング有無
		blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;



		// ブレンディング係数の設定
		switch (blendMode_) {
		case BlendMode::kNone:
			blendDesc.RenderTarget[0].BlendEnable = FALSE;
			break;
		case BlendMode::kNormal:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case BlendMode::kAdd:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kSubtract:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kMultily:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		}


#pragma endregion

#pragma region RasiterzerState

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region PSO

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = skyboxRootSignature_.Get(); // RootSignature

		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;         // InputLayout

		graphicsPipelineStateDesc.VS = {
			skyboxVertexShaderBlob_->GetBufferPointer(),
			skyboxVertexShaderBlob_->GetBufferSize() }; // VertexShader
		graphicsPipelineStateDesc.PS = {
			skyboxPixelShaderBlob_->GetBufferPointer(),
			skyboxPixelShaderBlob_->GetBufferSize() }; // PixelShader

		graphicsPipelineStateDesc.BlendState = blendDesc;           // BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		// 書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

#pragma endregion

		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateGraphicsPipelineState(
			&graphicsPipelineStateDesc, IID_PPV_ARGS(&skyboxPipelineState_));
		assert(SUCCEEDED(hr_));

		return skyboxPipelineState_;
	}
}


Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreateLineGraphicsPipeline(BlendMode blendMode_) {
	if (linePipelineState_) {
		return linePipelineState_;
	}
	else {
		linePipelineState_ = nullptr;

#pragma region InputLayout

		// InputLayoutの設定
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

#pragma endregion

#pragma region BlendState
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		// すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = FALSE; // アンチエイリアシング有無
		blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;



		// ブレンディング係数の設定
		switch (blendMode_) {
		case BlendMode::kNone:
			blendDesc.RenderTarget[0].BlendEnable = FALSE;
			break;
		case BlendMode::kNormal:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case BlendMode::kAdd:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kSubtract:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kMultily:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		}


#pragma endregion

#pragma region RasiterzerState

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region PSO

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = lineRootSignature_.Get(); // RootSignature

		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;         // InputLayout

		graphicsPipelineStateDesc.VS = {
			vertexShaderBlob_->GetBufferPointer(),
			vertexShaderBlob_->GetBufferSize() }; // VertexShader
		graphicsPipelineStateDesc.PS = {
			linePixelShaderBlob_->GetBufferPointer(),
			linePixelShaderBlob_->GetBufferSize() }; // PixelShader

		graphicsPipelineStateDesc.BlendState = blendDesc;           // BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。線
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		// 書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

#pragma endregion

		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateGraphicsPipelineState(
			&graphicsPipelineStateDesc, IID_PPV_ARGS(&linePipelineState_));
		assert(SUCCEEDED(hr_));

		return linePipelineState_;
	}
}


Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreateSpritePipeline(BlendMode blendMode_) {
	if (spritePipelineState_) {
		return spritePipelineState_;
	}
	else {
		spritePipelineState_ = nullptr;

#pragma region InputLayout

		// InputLayoutの設定
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

#pragma endregion

#pragma region BlendState
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		// すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = FALSE; // アンチエイリアシング有無
		blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

		// ブレンディング係数の設定
		switch (blendMode_) {
		case BlendMode::kNone:
			blendDesc.RenderTarget[0].BlendEnable = FALSE;
			break;
		case BlendMode::kNormal:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case BlendMode::kAdd:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kSubtract:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kMultily:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		}

#pragma endregion

#pragma region RasiterzerState

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region PSO

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = spriteRootSignature_.Get(); // RootSignature

		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; // InputLayout

		graphicsPipelineStateDesc.VS = {
			vertexSpriteShaderBlob_->GetBufferPointer(),
			vertexSpriteShaderBlob_->GetBufferSize() }; // VertexShader
		graphicsPipelineStateDesc.PS = {
			pixelSpriteShaderBlob_->GetBufferPointer(),
			pixelSpriteShaderBlob_->GetBufferSize() }; // PixelShader

		graphicsPipelineStateDesc.BlendState = blendDesc;           // BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		// 書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

#pragma endregion

		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateGraphicsPipelineState(
			&graphicsPipelineStateDesc, IID_PPV_ARGS(&spritePipelineState_));
		assert(SUCCEEDED(hr_));

		return spritePipelineState_;
	}
}

Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreateParticleGraphicsPipeline(BlendMode blendMode_) {
	if (particlesPipelineState_) {
		return particlesPipelineState_;
	}
	else {
		particlesPipelineState_ = nullptr;

#pragma region InputLayout

		// InputLayoutの設定
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[4] = {};
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
		inputElementDescs[3].SemanticName = "COLOR";
		inputElementDescs[3].SemanticIndex = 0;
		inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);

#pragma endregion

#pragma region BlendState
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		// すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = FALSE; // アンチエイリアシング有無
		blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

		// ブレンディング係数の設定
		switch (blendMode_) {
		case BlendMode::kNone:
			blendDesc.RenderTarget[0].BlendEnable = FALSE;
			break;
		case BlendMode::kNormal:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case BlendMode::kAdd:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kSubtract:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kMultily:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		}

#pragma endregion

#pragma region RasiterzerState

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示する
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region PSO

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = particleRootSignature_.Get(); // RootSignature

		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc; // InputLayout

		graphicsPipelineStateDesc.VS = {
		   particleVertexShaderBlob_->GetBufferPointer(),
		   particleVertexShaderBlob_->GetBufferSize() }; // VertexShader
		graphicsPipelineStateDesc.PS = {
			 particlePixelShaderBlob_->GetBufferPointer(),
			 particlePixelShaderBlob_->GetBufferSize() }; // PixelShader

		graphicsPipelineStateDesc.BlendState = blendDesc;           // BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		// 書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

#pragma endregion

		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateGraphicsPipelineState(
			&graphicsPipelineStateDesc, IID_PPV_ARGS(&particlesPipelineState_));
		assert(SUCCEEDED(hr_));

		return particlesPipelineState_;
	}
}

Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreateAnimationGraphicsPipeline(BlendMode blendMode_) {
	if (animationPipelineState_) {
		return animationPipelineState_;
	}
	else {
		animationPipelineState_ = nullptr;

#pragma region InputLayout

		// InputLayoutの設定
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[5] = {};

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
		inputElementDescs[3].SemanticName = "WEIGHT";
		inputElementDescs[3].SemanticIndex = 0;
		inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[3].InputSlot = 1;
		inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[4].SemanticName = "INDEX";
		inputElementDescs[4].SemanticIndex = 0;
		inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
		inputElementDescs[4].InputSlot = 1;
		inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;



		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);


#pragma endregion

#pragma region BlendState
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		// すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = FALSE; // アンチエイリアシング有無
		blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;


		// ブレンディング係数の設定
		switch (blendMode_) {
		case BlendMode::kNone:
			blendDesc.RenderTarget[0].BlendEnable = FALSE;
			break;
		case BlendMode::kNormal:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			break;
		case BlendMode::kAdd:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kSubtract:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			break;
		case BlendMode::kMultily:
			blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
			break;
		}


#pragma endregion

#pragma region RasiterzerState

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region PSO

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = animationRootSignature_.Get(); // RootSignature


		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;         // InputLayout

		graphicsPipelineStateDesc.VS = {
			animationVertexShaderBlob_->GetBufferPointer(),
			animationVertexShaderBlob_->GetBufferSize() }; // VertexShader
		graphicsPipelineStateDesc.PS = {
			pixelShaderBlob_->GetBufferPointer(),
			pixelShaderBlob_->GetBufferSize() }; // PixelShader


		graphicsPipelineStateDesc.BlendState = blendDesc;           // BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する

		depthStencilDesc.DepthEnable = true;
		// 書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


#pragma endregion

		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateGraphicsPipelineState(
			&graphicsPipelineStateDesc, IID_PPV_ARGS(&animationPipelineState_));
		assert(SUCCEEDED(hr_));

		return animationPipelineState_;
	}
}

Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreateAnimationCSGraphicsPipeline() {
	if (animationCSPipelineState_) {
		return animationCSPipelineState_;
	}
	else {
		animationCSPipelineState_ = nullptr;

#pragma region PSO

		// PSO生成
		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc{};

		computePipelineStateDesc.CS= {
			.pShaderBytecode = animationComputeShaderBlob_->GetBufferPointer(),
			.BytecodeLength = animationComputeShaderBlob_->GetBufferSize() };
#pragma endregion
		computePipelineStateDesc.pRootSignature = animationCSRootSignature_.Get();
		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateComputePipelineState(
			&computePipelineStateDesc, IID_PPV_ARGS(&animationCSPipelineState_));
		assert(SUCCEEDED(hr_));

		return animationCSPipelineState_;
	}
}


Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreatePostEffectGraphicsPipeline(Microsoft::WRL::ComPtr<IDxcBlob> postEffectPixelShaderBlob_) {
	Microsoft::WRL::ComPtr<ID3D12PipelineState> postEffectPipelineState_ = nullptr;
	if (postEffectPipelineState_) {
		return postEffectPipelineState_;
	}
	else {
		postEffectPipelineState_ = nullptr;

#pragma region InputLayout

		/*/ InputLayoutの設定
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		*/
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = nullptr;
		inputLayoutDesc.NumElements = 0;

#pragma endregion

#pragma region BlendState
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		// すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = FALSE; // アンチエイリアシング有無
		blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].BlendEnable = FALSE;


#pragma endregion

#pragma region RasiterzerState

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region PSO

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = postEffectRootSignature_.Get(); // RootSignature

		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;         // InputLayout

		graphicsPipelineStateDesc.VS = {
			postEffectVertexShaderBlob_->GetBufferPointer(),
			postEffectVertexShaderBlob_->GetBufferSize() }; // VertexShader
		graphicsPipelineStateDesc.PS = {
			postEffectPixelShaderBlob_->GetBufferPointer(),
			postEffectPixelShaderBlob_->GetBufferSize() }; // PixelShader

		graphicsPipelineStateDesc.BlendState = blendDesc;           // BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = false;

		/*/ 書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		*/

#pragma endregion

		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateGraphicsPipelineState(
			&graphicsPipelineStateDesc, IID_PPV_ARGS(&postEffectPipelineState_));
		assert(SUCCEEDED(hr_));

		return postEffectPipelineState_;
	}
}

Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreateOutLineGraphicsPipeline() {
	if (outLinePipelineState_) {
		return outLinePipelineState_;
	}
	else {
		outLinePipelineState_ = nullptr;

#pragma region InputLayout

		
		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = nullptr;
		inputLayoutDesc.NumElements = 0;

#pragma endregion

#pragma region BlendState
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		// すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = FALSE; // アンチエイリアシング有無
		blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].BlendEnable = FALSE;


#pragma endregion

#pragma region RasiterzerState

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region PSO

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = outLineRootSignature_.Get(); // RootSignature

		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;         // InputLayout

		graphicsPipelineStateDesc.VS = {
			postEffectVertexShaderBlob_->GetBufferPointer(),
			postEffectVertexShaderBlob_->GetBufferSize() }; // VertexShader
		graphicsPipelineStateDesc.PS = {
			outLinePixelShaderBlob_->GetBufferPointer(),
			outLinePixelShaderBlob_->GetBufferSize() }; // PixelShader

		graphicsPipelineStateDesc.BlendState = blendDesc;           // BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = false;

		/*/ 書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		*/
#pragma endregion

		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateGraphicsPipelineState(
			&graphicsPipelineStateDesc, IID_PPV_ARGS(&outLinePipelineState_));
		assert(SUCCEEDED(hr_));

		return outLinePipelineState_;
	}
}

Microsoft::WRL::ComPtr<ID3D12PipelineState>GraphicsPipeline::CreateDissolveGraphicsPipeline() {
	if (dissolvePipelineState_) {
		return dissolvePipelineState_;
	}
	else {
		dissolvePipelineState_ = nullptr;

#pragma region InputLayout


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = nullptr;
		inputLayoutDesc.NumElements = 0;

#pragma endregion

#pragma region BlendState
		// BlendStateの設定
		D3D12_BLEND_DESC blendDesc{};
		// すべての色要素を書き込む
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendDesc.AlphaToCoverageEnable = FALSE; // アンチエイリアシング有無
		blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無

		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].BlendEnable = FALSE;


#pragma endregion

#pragma region RasiterzerState

		// RasiterzerStateの設定
		D3D12_RASTERIZER_DESC rasterizerDesc{};
		// 裏面(時計回り)を表示しない
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		// 三角形の中を塗りつぶす
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

#pragma region PSO

		// PSO生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = dissolveRootSignature_.Get(); // RootSignature

		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;         // InputLayout

		graphicsPipelineStateDesc.VS = {
			postEffectVertexShaderBlob_->GetBufferPointer(),
			postEffectVertexShaderBlob_->GetBufferSize() }; // VertexShader
		graphicsPipelineStateDesc.PS = {
			dissolvePixelShaderBlob_->GetBufferPointer(),
			dissolvePixelShaderBlob_->GetBufferSize() }; // PixelShader

		graphicsPipelineStateDesc.BlendState = blendDesc;           // BrendState
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトロポジ（形状）のタイプ。三角形
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に色を打ち込むの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// DepthStencilStateの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = false;

		// 書き込みします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// DepthStencilの設定
		graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		
#pragma endregion

		// 実際に生成
		HRESULT hr_ = Engine::GetDevice()->CreateGraphicsPipelineState(
			&graphicsPipelineStateDesc, IID_PPV_ARGS(&dissolvePipelineState_));
		assert(SUCCEEDED(hr_));

		return dissolvePipelineState_;
	}
}


Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreateRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->rootSignature_) {
		return GraphicsPipeline::GetInstance()->rootSignature_;
	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE Range[1] = {};
	Range[0].BaseShaderRegister = 1;                      // 0から始まる
	Range[0].NumDescriptors = 1;                          // 数は1つ
	Range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	Range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// RootSignature作成. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[7] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//worldTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//Texture
	rootParameters[2].ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

	//Light
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号1を使う

	// viewProjection
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // vertexShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1; // レジスタ番号を1にバインド

	//CameraWorldPos
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[5].Descriptor.ShaderRegister = 2; // レジスタ番号2を使う

	//環境マップ用
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[6].DescriptorTable.pDescriptorRanges = Range; // Tableの中身の配列を指定
	rootParameters[6].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
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
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->rootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(
		0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->rootSignature_));
	assert(SUCCEEDED(hr_));


	return rootSignature_;

#pragma endregion

}

Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreateSkyboxRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->skyboxRootSignature_) {
		return GraphicsPipeline::GetInstance()->skyboxRootSignature_;
	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算



	// RootSignature作成. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	//material
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//worldTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//Texture
	rootParameters[2].ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges =
		descriptorRange; // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges =
		_countof(descriptorRange); // Tableで利用する数

	// viewProjection
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // vertexShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号を1にバインド


	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
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
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->skyboxRootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(
		0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->skyboxRootSignature_));
	assert(SUCCEEDED(hr_));


	return skyboxRootSignature_;

#pragma endregion

}

Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreateLineRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->lineRootSignature_) {
		return GraphicsPipeline::GetInstance()->lineRootSignature_;
	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算



	// RootSignature作成. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//worldTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//Light
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 1; // レジスタ番号1を使う

	// viewProjection
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // vertexShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号を1にバインド

	//CameraWorldPos
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 2; // レジスタ番号2を使う

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
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
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->lineRootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(
		0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->lineRootSignature_));
	assert(SUCCEEDED(hr_));


	return lineRootSignature_;

#pragma endregion

}

Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreateSpriteRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->spriteRootSignature_) {
		return GraphicsPipeline::GetInstance()->spriteRootSignature_;
	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算



	// RootSignature作成. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[6] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//worldTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//Texture
	rootParameters[2].ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

	//Light
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号1を使う

	// viewProjection
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // vertexShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1; // レジスタ番号を1にバインド

	//CameraWorldPos
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[5].Descriptor.ShaderRegister = 2; // レジスタ番号2を使う

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
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
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->spriteRootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(
		0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->spriteRootSignature_));
	assert(SUCCEEDED(hr_));


	return spriteRootSignature_;

#pragma endregion

}

Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreateParticleRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->particleRootSignature_) {
		return GraphicsPipeline::GetInstance()->particleRootSignature_;
	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// RootSignature作成. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	rootParameters[1].ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges =
		descriptorRange; // Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges =
		_countof(descriptorRange); // Tableで利用する数

	rootParameters[2].ParameterType =
		D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges =
		descriptorRange; // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges =
		_countof(descriptorRange); // Tableで利用する数

	// worldTransform
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // vertexShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 0; // レジスタ番号を1にバインド

	// viewProjection
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // vertexShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1; // レジスタ番号を1にバインド

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
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
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->particleRootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(
		0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->particleRootSignature_));
	assert(SUCCEEDED(hr_));

	return particleRootSignature_;

#pragma endregion
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreateAnimationRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->animationRootSignature_) {
		return GraphicsPipeline::GetInstance()->animationRootSignature_;

	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
	D3D12_DESCRIPTOR_RANGE Range[1] = {};
	Range[0].BaseShaderRegister = 1;                      // 0から始まる
	Range[0].NumDescriptors = 1;                          // 数は1つ
	Range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	Range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// RootSignature作成. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[8] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//worldTransform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	//Texture
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

	//Light
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号1を使う

	// viewProjection
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // vertexShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1; // レジスタ番号を1にバインド

	//CameraWorldPos
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[5].Descriptor.ShaderRegister = 2; // レジスタ番号2を使う

	//Well
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;    // tabelを使う
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // Vertexhaderで使う
	rootParameters[6].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
	rootParameters[6].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

	//環境マップ用
	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[7].DescriptorTable.pDescriptorRanges = Range; // Tableの中身の配列を指定
	rootParameters[7].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数


	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
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
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->animationRootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(
		0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->animationRootSignature_));
	assert(SUCCEEDED(hr_));


	return animationRootSignature_;


#pragma endregion

}

Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreateAnimationCSRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->animationCSRootSignature_) {
		return GraphicsPipeline::GetInstance()->animationCSRootSignature_;

	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRangeSRV[3] = {};
	// gMatrixPalette
	descriptorRangeSRV[0].BaseShaderRegister = 0;
	descriptorRangeSRV[0].NumDescriptors = 1;
	descriptorRangeSRV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeSRV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Vertices
	descriptorRangeSRV[1].BaseShaderRegister = 1;
	descriptorRangeSRV[1].NumDescriptors = 1;
	descriptorRangeSRV[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeSRV[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// Influences
	descriptorRangeSRV[2].BaseShaderRegister = 2;
	descriptorRangeSRV[2].NumDescriptors = 1;
	descriptorRangeSRV[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeSRV[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	D3D12_DESCRIPTOR_RANGE descriptorRangeUAV[1] = {};
	// gOutputVertices
	descriptorRangeUAV[0].BaseShaderRegister = 0;
	descriptorRangeUAV[0].NumDescriptors = 1;
	descriptorRangeUAV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	descriptorRangeUAV[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// RootSignature作成. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	// gMatrixPalette
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRangeSRV[0];
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;

	// Vertices
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRangeSRV[1];
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;

	// gInfluences
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRangeSRV[2];
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;

	//Vertex書き込み用
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[3].DescriptorTable.pDescriptorRanges = &descriptorRangeUAV[0];
	rootParameters[3].DescriptorTable.NumDescriptorRanges = 1;

	//Skinning
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // ComputeShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド


	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけMipmapを使う
	staticSamplers[0].ShaderRegister = 0;         // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->animationCSRootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(
		0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->animationCSRootSignature_));
	assert(SUCCEEDED(hr_));


	return animationCSRootSignature_;


#pragma endregion

}


Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreatePostEffectRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->postEffectRootSignature_) {
		return GraphicsPipeline::GetInstance()->postEffectRootSignature_;
	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// RootSignature. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); // Tableで利用する数

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号1を使う


	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけMipmapを使う
	staticSamplers[0].ShaderRegister = 0;         // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->postEffectRootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->postEffectRootSignature_));
	assert(SUCCEEDED(hr_));


	return postEffectRootSignature_;

#pragma endregion

}

Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreateOutLineRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->outLineRootSignature_) {
		return GraphicsPipeline::GetInstance()->outLineRootSignature_;
	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE descriptorDepth[1] = {};
	descriptorDepth[0].BaseShaderRegister = 1;                      // 1から始まる
	descriptorDepth[0].NumDescriptors = 1;                          // 数は1つ
	descriptorDepth[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorDepth[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// RootSignature. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//レンダーテクスチャー用
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRange; // Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号1を使う
	//ディプステクスチャー用
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorDepth; // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

	
	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[2] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけMipmapを使う
	staticSamplers[0].ShaderRegister = 0;         // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT; // バイリニアフィルタ
	staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP; // 0~1の範囲外をリピート
	staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX; // ありったけMipmapを使う
	staticSamplers[1].ShaderRegister = 1;         // レジスタ番号0を使う
	staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // Pi
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->outLineRootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->outLineRootSignature_));
	assert(SUCCEEDED(hr_));


	return outLineRootSignature_;

#pragma endregion

}

Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::CreateDissolveRootSignature() {

	//	ルートシグネチャーがあれば渡す
	if (GraphicsPipeline::GetInstance()->dissolveRootSignature_) {
		return GraphicsPipeline::GetInstance()->dissolveRootSignature_;
	}

#pragma region RootSignature

	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	descriptorRange[0].BaseShaderRegister = 0;                      // 0から始まる
	descriptorRange[0].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
	descriptorRange[1].BaseShaderRegister = 1;                      // 0から始まる
	descriptorRange[1].NumDescriptors = 1;                          // 数は1つ
	descriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// RootSignature. 複数設定できるので配列。
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRange[0]; // Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; // レジスタ番号1を使う

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRange[1]; // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数


	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	descriptionRootSignature.pParameters = rootParameters; // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); // 配列の長さ

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP; // 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけMipmapを使う
	staticSamplers[0].ShaderRegister = 0;         // レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	HRESULT hr_ = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Utility::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	// バイナリを元に生成
	GraphicsPipeline::GetInstance()->dissolveRootSignature_ = nullptr;
	hr_ = Engine::GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&GraphicsPipeline::GetInstance()->dissolveRootSignature_));
	assert(SUCCEEDED(hr_));


	return dissolveRootSignature_;

#pragma endregion

}



Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateVSShader() {
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
	// Shaderをコンパイルする
	//	早期リターン
	if (GraphicsPipeline::GetInstance()->vertexShaderBlob_) {
		return GraphicsPipeline::GetInstance()->vertexShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->vertexShaderBlob_ =
		CompileShader(L"resources/hlsl/Object3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob_ != nullptr);
	return GraphicsPipeline::GetInstance()->vertexShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreatePSShader() {
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

	// Shaderをコンパイルする
	if (GraphicsPipeline::GetInstance()->pixelShaderBlob_) {
		return GraphicsPipeline::GetInstance()->pixelShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->pixelShaderBlob_ =
		CompileShader(L"resources/hlsl/Object3d.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob_ != nullptr);

	return GraphicsPipeline::GetInstance()->pixelShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateSkyboxVSShader() {
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
	// Shaderをコンパイルする
	//	早期リターン
	if (GraphicsPipeline::GetInstance()->skyboxVertexShaderBlob_) {
		return GraphicsPipeline::GetInstance()->skyboxVertexShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->skyboxVertexShaderBlob_ =
		CompileShader(L"resources/hlsl/Skybox.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(skyboxVertexShaderBlob_ != nullptr);
	return GraphicsPipeline::GetInstance()->skyboxVertexShaderBlob_;
}
//	SkyCubepixelshaderの生成
Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateSkyboxPSShader() {
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

	// Shaderをコンパイルする
	if (GraphicsPipeline::GetInstance()->skyboxPixelShaderBlob_) {
		return GraphicsPipeline::GetInstance()->skyboxPixelShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->skyboxPixelShaderBlob_ =
		CompileShader(L"resources/hlsl/Skybox.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(skyboxPixelShaderBlob_ != nullptr);

	return GraphicsPipeline::GetInstance()->skyboxPixelShaderBlob_;
};

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateLinePSShader() {
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

	// Shaderをコンパイルする
	if (GraphicsPipeline::GetInstance()->linePixelShaderBlob_) {
		return GraphicsPipeline::GetInstance()->linePixelShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->linePixelShaderBlob_ =
		CompileShader(L"resources/hlsl/Line.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(linePixelShaderBlob_ != nullptr);

	return GraphicsPipeline::GetInstance()->linePixelShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateSpriteVSShader() {
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
	// Shaderをコンパイルする
	//	早期リターン
	if (GraphicsPipeline::GetInstance()->vertexSpriteShaderBlob_) {
		return GraphicsPipeline::GetInstance()->vertexSpriteShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->vertexSpriteShaderBlob_ = CompileShader(
		L"resources/hlsl/SpriteVS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexSpriteShaderBlob_ != nullptr);
	return GraphicsPipeline::GetInstance()->vertexSpriteShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateSpritePSShader() {
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

	// Shaderをコンパイルする
	if (GraphicsPipeline::GetInstance()->pixelSpriteShaderBlob_) {
		return GraphicsPipeline::GetInstance()->pixelSpriteShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->pixelSpriteShaderBlob_ = CompileShader(
		L"resources/hlsl/SpritePS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelSpriteShaderBlob_ != nullptr);

	return GraphicsPipeline::GetInstance()->pixelSpriteShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateParticleVSShader() {
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
	// Shaderをコンパイルする
	//	早期リターン
	if (GraphicsPipeline::GetInstance()->particleVertexShaderBlob_) {
		return GraphicsPipeline::GetInstance()->particleVertexShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->particleVertexShaderBlob_ =
		CompileShader(L"resources/hlsl/Particle.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(particleVertexShaderBlob_ != nullptr);
	return GraphicsPipeline::GetInstance()->particleVertexShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateParticlePSShader() {
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

	// Shaderをコンパイルする
	if (GraphicsPipeline::GetInstance()->particlePixelShaderBlob_) {
		return GraphicsPipeline::GetInstance()->particlePixelShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->particlePixelShaderBlob_ =
		CompileShader(L"resources/hlsl/Particle.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(particlePixelShaderBlob_ != nullptr);

	return GraphicsPipeline::GetInstance()->particlePixelShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateAnimationVSShader() {

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
	// Shaderをコンパイルする
	//	早期リターン
	if (GraphicsPipeline::GetInstance()->animationVertexShaderBlob_) {
		return GraphicsPipeline::GetInstance()->animationVertexShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->animationVertexShaderBlob_ =
		CompileShader(L"resources/hlsl/SkinningObject3d.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(animationVertexShaderBlob_ != nullptr);
	return GraphicsPipeline::GetInstance()->animationVertexShaderBlob_;

}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateAnimationCSShader() {

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
	// Shaderをコンパイルする
	//	早期リターン
	if (GraphicsPipeline::GetInstance()->animationComputeShaderBlob_) {
		return GraphicsPipeline::GetInstance()->animationComputeShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->animationComputeShaderBlob_ =
		CompileShader(L"resources/hlsl/Skinning.CS.hlsl", L"cs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(animationComputeShaderBlob_ != nullptr);
	return GraphicsPipeline::GetInstance()->animationComputeShaderBlob_;

}


Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreatePostEffectVSShader() {
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
	// Shaderをコンパイルする
	//	早期リターン
	if (GraphicsPipeline::GetInstance()->postEffectVertexShaderBlob_) {
		return GraphicsPipeline::GetInstance()->postEffectVertexShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->postEffectVertexShaderBlob_ =
		CompileShader(L"resources/hlsl/Fullscreen.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(postEffectVertexShaderBlob_ != nullptr);
	return GraphicsPipeline::GetInstance()->postEffectVertexShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreatePostEffectPSShader(const std::wstring path) {
	HRESULT hr_ = S_FALSE;

	Microsoft::WRL::ComPtr<IDxcBlob> postEffectPixelShaderBlob_ = nullptr;

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

	// Shaderをコンパイルする
	if (postEffectPixelShaderBlob_) {
		return postEffectPixelShaderBlob_;
	}
	postEffectPixelShaderBlob_ = CompileShader(L"resources/hlsl/" + path, L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(postEffectPixelShaderBlob_ != nullptr);

	return postEffectPixelShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateOutLinePSShader() {
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

	// Shaderをコンパイルする
	if (GraphicsPipeline::GetInstance()->outLinePixelShaderBlob_) {
		return GraphicsPipeline::GetInstance()->outLinePixelShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->outLinePixelShaderBlob_ =
		CompileShader(L"resources/hlsl/OutLine.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(outLinePixelShaderBlob_ != nullptr);

	return GraphicsPipeline::GetInstance()->outLinePixelShaderBlob_;
}

Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::CreateDissolvePSShader() {
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

	// Shaderをコンパイルする
	if (GraphicsPipeline::GetInstance()->dissolvePixelShaderBlob_) {
		return GraphicsPipeline::GetInstance()->dissolvePixelShaderBlob_;
	}
	GraphicsPipeline::GetInstance()->dissolvePixelShaderBlob_ =
		CompileShader(L"resources/hlsl/Dissolve.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(dissolvePixelShaderBlob_ != nullptr);

	return GraphicsPipeline::GetInstance()->dissolvePixelShaderBlob_;
}



IDxcBlob* GraphicsPipeline::CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したものを3つ
	IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler) {
	// これからシェーダーをコンパイルする旨をログに出す
	Utility::Log(Utility::ConvertString(
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
		Utility::Log(shaderError->GetStringPointer());
		// 警告・エラーダメ絶対
		assert(false);
	}

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	Utility::Log(Utility::ConvertString(
		std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを返却
	return shaderBlob;
}