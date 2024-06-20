#include "FullScreen.hlsli"

struct OutLineStyle
{
    
    int32_t isEnable;
    float32_t differenceValue;
    float32_t4x4 projectionInverse;
};


Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
Texture2D<float32_t> gDepthTexture : register(t1);
SamplerState gSamplerPoint : register(s1);
ConstantBuffer<OutLineStyle> gOutLineStyle : register(b0);


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

static const float32_t2 kIndex3x3[3][3] =
{
    { float32_t2(-1.0f, -1.0f), float32_t2(0.0f, -1.0f), float32_t2(1.0f, -1.0f) },
    { float32_t2(-1.0f, 0.0f), float32_t2(0.0f, 0.0f), float32_t2(1.0f, 0.0f) },
    { float32_t2(-1.0f, 1.0f), float32_t2(0.0f, 1.0f), float32_t2(1.0f, 1.0f) }
};

static const float32_t kPrewittHorizontalKernel[3][3] ={
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float32_t kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f/6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f/6.0f, 1.0f / 6.0f },
};

float32_t Luminance(float32_t3 v)
{
    return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));

}

PixelShaderOutput main(VertexShaderOutput input)
{
   
    PixelShaderOutput output;
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    float32_t2 difference = float32_t2(0.0f, 0.0f);
 
    
    if (gOutLineStyle.isEnable != 0){
        for (int32_t x = 0; x < 3; ++x)
        {
            for (int32_t y = 0; y < 3; ++y)
            {
                float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
                float32_t ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
                float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), (float32_t4x4)gOutLineStyle.projectionInverse);
                float32_t viewZ = viewSpace.z * rcp(viewSpace.w);
                difference.x += viewZ * kPrewittHorizontalKernel[x][y];
                difference.y += viewZ * kPrewittVerticalKernel[x][y];
            }

        }
        float32_t weight = length(difference);
        weight = saturate(weight /* gOutLineStyle.differenceValue*/);//0～1の範囲に収める
        //weightが大きければ多き程暗く表示する
        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler,input.texcoord).rgb;
        output.color.a = 1.0f;
    }
    
   
    
    return output;
}