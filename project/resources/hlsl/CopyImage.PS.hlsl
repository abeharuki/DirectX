#include "FullScreen.hlsli"

struct Grayscale
{
    int32_t isEnable;
};

struct Vignetting
{
    int32_t isEnable;
    float32_t3 color;
    float intensity;
};

struct WritingStyle
{
    Grayscale grayscale;
    Vignetting vignetting;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<WritingStyle> gWritingStyle : register(b0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    if (gWritingStyle.grayscale.isEnable != 0)
    {
        float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
        output.color.rgb = float32_t3(value, value, value);

    }
    
    if (gWritingStyle.vignetting.isEnable != 0)
    {
        float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
        float vignetting = correct.x * correct.y * gWritingStyle.vignetting.intensity;
        vignetting = saturate(pow(vignetting, 0.8f));
        output.color.rgb *= vignetting;
        output.color.rgb += gWritingStyle.vignetting.color;

    }
    
    return output;
}