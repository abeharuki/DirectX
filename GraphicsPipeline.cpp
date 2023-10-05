#include "GraphicsPipeline.h"

// ルートシグネチャ
Microsoft::WRL::ComPtr<ID3D12RootSignature> GraphicsPipeline::rootSignature_;
// パイプラインステートオブジェクト
Microsoft::WRL::ComPtr<ID3D12PipelineState> GraphicsPipeline::sPipelineState_;
Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::vertexShaderBlob_;
Microsoft::WRL::ComPtr<IDxcBlob> GraphicsPipeline::pixelShaderBlob_;

GraphicsPipeline* GraphicsPipeline::GetInstance() {
	static GraphicsPipeline Instance;
	return &Instance;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> GraphicsPipeline::CreateGraphicsPipeline() {
	if (sPipelineState_) {
		return sPipelineState_;
	} else {
		sPipelineState_ = nullptr;
	}
#pragma region InputLayout

	//InputLayoutの設定
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
	blendDesc.AlphaToCoverageEnable = FALSE;  // アンチエイリアシング有無
	blendDesc.IndependentBlendEnable = FALSE; // ブレンドステートを個別化するか有無
	blendDesc.RenderTarget[0].BlendEnable = TRUE; // ブレンディング有無
	// ブレンディング係数の設定
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
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
	    vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize()}; // VertexShader
	graphicsPipelineStateDesc.PS = {
	    pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize()}; // PixelShader

	graphicsPipelineStateDesc.BlendState = blendDesc;                             // BrendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;                   // RasterizerState

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
	HRESULT hr_ = DirectXCommon::GetDevice()->CreateGraphicsPipelineState(
	    &graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineState_));
	assert(SUCCEEDED(hr_));

}
