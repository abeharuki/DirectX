#include "Sprite.hlsli"


struct DissolveStyle
{
    float32_t4x4 uvTransform;
    float32_t threshold;
    float32_t3 edgeColor;
    int32_t isGradient;
    int32_t isEnble;
};



ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DissolveStyle> gDissolve : register(b1);
Texture2D<float32_t> gMaskTexture : register(t1);

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
	float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    float32_t4 finalColor = { 0, 0, 0, 1 };
	
	if (textureColor.a == 0.0) {
		discard;
	}

    float4 maskUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gDissolve.uvTransform);
    float32_t mask = gMaskTexture.Sample(gSampler, maskUV.xy);
    
    if (gDissolve.isGradient != 0)
    {
        // アルファ値を計算
        finalColor.a = saturate((mask - gDissolve.threshold) / (1.0f - gDissolve.threshold));
        if (finalColor.a < 0.01f)
        {
            discard;
        }
    }
    else
    {
        //maskの値が0.5（閾値）以下の場合はdiscardして抜く
        if (mask < gDissolve.threshold)
        {
            discard;
        }
           
    }
    
    finalColor.rgb = gMaterial.color.rgb * textureColor.rgb;
	
     //Edgeっぽさほ算出
    float32_t edge = 1.0f - smoothstep(gDissolve.threshold, gDissolve.threshold + 0.03f, mask);
  
    //Edgeっぽいほど指定した色を加算
    finalColor.rgb += edge * gDissolve.edgeColor;
    
    output.color.rgb = finalColor.rgb;
    output.color.a = gMaterial.color.a * textureColor.a * finalColor.a;
    if (output.color.a == 0.0)
    {
        discard;
    }
	return output;
}
