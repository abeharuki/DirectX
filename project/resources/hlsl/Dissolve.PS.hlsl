#include "FullScreen.hlsli"

struct DissolveStyle{
    float32_t threshold;
    float32_t3 edgeColor;
    int32_t isEnble;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DissolveStyle> gDissolveStyle : register(b0);
Texture2D<float32_t> gMaskTexture : register(t1);


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};



PixelShaderOutput main(VertexShaderOutput input)
{
   
    PixelShaderOutput output;
    
  
    if (gDissolveStyle.isEnble != 0){
        float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
        //maskの値が0.5（閾値）以下の場合はdiscardして抜く
        if (mask < gDissolveStyle.threshold){
            discard;
        }
        //Edgeっぽさほ算出
        float32_t edge = 1.0f - smoothstep(gDissolveStyle.threshold, gDissolveStyle.threshold + 0.03f, mask);
        output.color = gTexture.Sample(gSampler, input.texcoord);
        
        //Edgeっぽいほど指定した色を加算
        output.color.rgb += edge * gDissolveStyle.edgeColor;
        
    }
    else
    {
        output.color = gTexture.Sample(gSampler, input.texcoord);
  
    }
    
    
    
    return output;
}