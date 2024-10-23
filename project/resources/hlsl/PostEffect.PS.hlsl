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

struct RadialBlur
{
    float32_t2 center;
    float32_t blurWidth;
    float32_t rotation;
    int32_t isEnble;
};

struct Random
{
    float32_t time;
    int32_t isEnble;
};

struct HSVMaterial
{
    float32_t hue;
    float32_t saturation;
    float32_t value;
};

struct PostEffectStyle
{
    Grayscale grayscale;
    Vignetting vignetting;
    Gaussian gaussian;
    RadialBlur radialBlur;
    Random random;
    HSVMaterial material;
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

float rand2dTo1d(float2 value)
{
    
    float2 smallValue = sin(value);
    float2 dotDir = float2(12.9898, 78.233);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}

float32_t3 RGBToHSV(float32_t3 rgb){
    float max = rgb.r > rgb.g ? rgb.r : rgb.g;
    max = max > rgb.b ? max : rgb.b;
    float min = rgb.r < rgb.g ? rgb.r : rgb.g;
    min = min < rgb.b ? min : rgb.b;
    float h = max - min;
    if (h > 0.0f)
    {
        if (max == rgb.r)
        {
            h = (rgb.g - rgb.b) / h;
            if (h < 0.0f)
            {
                h += 6.0f;
            }
        }
        else if (max == rgb.g)
        {
            h = 2.0f + (rgb.b - rgb.r) / h;
        }
        else
        {
            h = 4.0f + (rgb.r - rgb.g) / h;
        }
    }
    
    h /= 6.0f;
    float s = (max - min);
    if (max != 0.0f)
        s /= max;
    float v = max;
    
    float32_t3 hsv = float32_t3(h,s,v);
    return hsv;
}

float32_t3 HSVToRGB(float32_t3 hsv){
    float r = hsv.z;
    float g = hsv.z;
    float b = hsv.z;
    if (hsv.y > 0.0f)
    {
        hsv.x *= 6.0f;
        int i = (int) hsv.x;
        float f = hsv.x - (float) i;
        switch (i)
        {
            default:
            case 0:
                g *= 1 - hsv.y * (1 - f);
                b *= 1 - hsv.y;
                break;
            case 1:
                r *= 1 - hsv.y * f;
                b *= 1 - hsv.y;
                break;
            case 2:
                r *= 1 - hsv.y;
                b *= 1 - hsv.y * (1 - f);
                break;
            case 3:
                r *= 1 - hsv.y;
                g *= 1 - hsv.y * f;
                break;
            case 4:
                r *= 1 - hsv.y * (1 - f);
                g *= 1 - hsv.y;
                break;
            case 5:
                g *= 1 - hsv.y;
                b *= 1 - hsv.y * f;
                break;
        }
    }
    
    float32_t3 rgb = float32_t3(r,g,b);
    return rgb;
    
}

float32_t WrapValue(float32_t value,float32_t minRange,float32_t maxRange)
{
    float32_t range = maxRange - minRange;
    float32_t modValue = fmod(value - minRange, range);
    if (modValue < 0)
    {
        modValue *= range;
    }
    return minRange + modValue;
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
    
     //ラジアルブラー
    if (gPostEffectStyle.radialBlur.isEnble != 0)
    {
        const float32_t2 kCenter = gPostEffectStyle.radialBlur.center;
        const int32_t kNumSamples = 10;
        const float32_t kBlurWidth = gPostEffectStyle.radialBlur.blurWidth;
        const float32_t kRotationStrength = gPostEffectStyle.radialBlur.rotation; // 渦巻きの回転強度
        
        //中心から現在のuvに対して方向を計算
        float32_t2 direction = input.texcoord - kCenter;
        float32_t3 outputColor = float32_t3(0.0f, 0.0f, 0.0f);

        for (int32_t sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
        {
            // サンプルごとの回転角度を計算
            float32_t angle = kRotationStrength * float32_t(sampleIndex); // サンプルごとの回転角度
            // 回転行列を使って方向ベクトルを回転させる
            float32_t2 rotatedDirection = float32_t2(
                direction.x * cos(angle) - direction.y * sin(angle),
                direction.x * sin(angle) + direction.y * cos(angle)
            );
            
            //現愛のuvから先ほどの計算した方向にサンプリング点を進めながらサンプリングしていく
            float32_t2 texcoord = input.texcoord + rotatedDirection * kBlurWidth * float32_t(sampleIndex);
            outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
        }

        //平均かする
        outputColor.rgb *= rcp(kNumSamples);
        
        output.color.rgb = outputColor;
        output.color.a = 1.0f;
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
    
     //ノイズ
    if (gPostEffectStyle.random.isEnble != 0)
    {
        float32_t random = rand2dTo1d(input.texcoord + gPostEffectStyle.random.time);
        float32_t4 randomValue = float32_t4(random, random, random, 1.0f);
        output.color *= randomValue;

    }
  
    float32_t3 hsv = RGBToHSV(output.color.rgb);
    hsv.x += gPostEffectStyle.material.hue;
    hsv.y += gPostEffectStyle.material.saturation;
    hsv.z += gPostEffectStyle.material.value;
    
    hsv.x = WrapValue(hsv.x, 0.0f, 1.0f);
    hsv.y = saturate(hsv.y);
    hsv.z = saturate(hsv.z);
    
    float32_t3 rgb = HSVToRGB(hsv);
    
    output.color.rgb = rgb;
    
    return output;
}