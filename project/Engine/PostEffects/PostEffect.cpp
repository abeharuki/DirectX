#include "PostEffect.h"

PostEffect* PostEffect::instance_ = nullptr;

PostEffect* PostEffect::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new PostEffect();
	}
	return instance_;
}

void PostEffect::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
	
}


void PostEffect::Initialize() {
	sPipeline();
	CreateResource();
	outline_ = std::make_unique<OutLine>();
	outline_->Initialize();

	dissolve_ = std::make_unique<Dissolve>();
	dissolve_->Initialize();

	gasianBlur_ = std::make_unique<GasianBlur>();
	gasianBlur_->Initialize();

	bloom_ = std::make_unique<Bloom>();
	bloom_->Initialize();
}

void PostEffect::Update() {}

void PostEffect::Draw() {
	++postEffectData->random.time;

	if (effect_) {

		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		Engine::GetList()->SetGraphicsRootSignature(rootSignature_.Get());
		Engine::GetList()->SetPipelineState(sPipelineState_.Get());

		// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
		Engine::GetList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Engine::GetList()->SetGraphicsRootDescriptorTable(0, Engine::GetInstance()->GetHandle());
		Engine::GetList()->SetGraphicsRootDescriptorTable(0, gasianBlur_->GetHandle());
		Engine::GetList()->SetGraphicsRootConstantBufferView(1, postEffectResource_->GetGPUVirtualAddress());

		// 三角形の描画
		Engine::GetList()->DrawInstanced(3, 1, 0, 0);
	}
	
}

void PostEffect::Apply() {
	outline_->Draw();
	dissolve_->Draw(outline_->GetHandle());
	bloom_->Draw(dissolve_->GetHandle());
	gasianBlur_->Draw(bloom_->GetHandle());
}

void PostEffect::CreateResource() {
	postEffectResource_ = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(PostEffects));
	postEffectResource_->Map(0, nullptr, reinterpret_cast<void**>(&postEffectData));
	//グレイスケール
	postEffectData->grayscal.isEnable = false;
	//ビネット
	postEffectData->vignetting.isEnable = false;
	postEffectData->vignetting.color = Vector3(0.0f, 0.0f, 0.0f);
	postEffectData->vignetting.intensity = 16.0f;
	//ガウシャン
	postEffectData->gaussian.isEnable = false;
	postEffectData->gaussian.kernelSize = false;
	//ラジアルブラー
	postEffectData->radialBlur.isEnble = false;
	postEffectData->radialBlur.center = { 0.5f,0.5f };
	postEffectData->radialBlur.blurWidth = 0.01f;
	postEffectData->radialBlur.rotation = 0.0f;



}

void PostEffect::sPipeline() {

	vertexShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePostEffectVSShader();
	pixelShaderBlob_ = GraphicsPipeline::GetInstance()->CreatePostEffectPSShader(L"PostEffect.PS.hlsl");


	rootSignature_ = GraphicsPipeline::GetInstance()->CreatePostEffectRootSignature();
	sPipelineState_ = GraphicsPipeline::GetInstance()->CreatePostEffectGraphicsPipeline(pixelShaderBlob_);

};