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

struct Gaussian
{
    int32_t isEnable;
    float32_t kernelSize;
};

struct PostEffectStyle
{
    Grayscale grayscale;
    Vignetting vignetting;
    Gaussian gaussian;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<PostEffectStyle> gPostEffectStyle : register(b0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);

}

static const float32_t2 kIndex3x3[3][3] =
{
    { float32_t2(-1.0f, -1.0f), float32_t2(0.0f, -1.0f), float32_t2(1.0f, -1.0f) },
    { float32_t2(-1.0f, 0.0f), float32_t2(0.0f, 0.0f), float32_t2(1.0f, 0.0f) },
    { float32_t2(-1.0f, 1.0f), float32_t2(0.0f, 1.0f), float32_t2(1.0f, 1.0f) }
};

static const float32_t2 kIndex5x5[5][5] =
{
    { { -2.0f, -2.0f }, { -1.0f, -2.0f }, { 0.0f, -2.0f }, { 1.0f, -2.0f }, { 2.0f, -2.0f } },
    { { -2.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, { 2.0f, -1.0f } },
    { { -2.0f, 0.0f }, { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 2.0f, 0.0f } },
    { { -2.0f, 1.0f }, { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 2.0f, 1.0f } },
    { { -2.0f, 2.0f }, { -1.0f, 2.0f }, { 0.0f, 2.0f }, { 1.0f, 2.0f }, { 2.0f, 2.0f } },
};





PixelShaderOutput main(VertexShaderOutput input)
{
   
    PixelShaderOutput output;
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    
    
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
    
    
   // float32_t sigma = gPostEffectStyle.smoothing.kernelSize;
    
   
    
    //ガウシアンフィルター
    if (gPostEffectStyle.gaussian.isEnable != 0)
    {
        output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
        output.color.a = 1.0f;
        
        float32_t weight = 0.0f;
        float32_t kernel3x3[3][3];
        for (int32_t x = 0; x < 3; ++x)
        {
            for (int32_t y = 0; y < 3; ++y)
            {
                kernel3x3[x][y] = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, gPostEffectStyle.gaussian.kernelSize);
                weight += kernel3x3[x][y];

            }

        }
       
        
        for (int32_t i = 0; i < 3; ++i)
        {
            for (int32_t j = 0; j < 3; ++j)
            {

                float32_t2 texcoord = input.texcoord + kIndex3x3[i][j] * uvStepSize;
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                output.color.rgb += fetchColor * kernel3x3[i][j];
            
            }

        }
        output.color.rgb *= rcp(weight);
    }
   
    //グレースケール
    if (gPostEffectStyle.grayscale.isEnable != 0)
    {
        float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
        output.color.rgb = float32_t3(value, value, value);

    }
    
    //ビネット
    if (gPostEffectStyle.vignetting.isEnable != 0)
    {
        float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
        float vignetting = correct.x * correct.y * gPostEffectStyle.vignetting.intensity;
        vignetting = saturate(pow(vignetting, 0.8f));
        output.color.rgb *= vignetting;
        output.color.rgb += gPostEffectStyle.vignetting.color;

    }
    
   
       
    
    return output;
}