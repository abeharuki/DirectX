struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t3 texcoord : TEXCOORE0;
};

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct TransformationMatrix
{
    float32_t4x4 matWorld;
    float32_t4x4 WorldInverseTranspose;
};



struct ViewProjectionMatrix
{
    float32_t4x4 view;
    float32_t4x4 projection;
    float32_t3 worldPosition;
};


struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};
