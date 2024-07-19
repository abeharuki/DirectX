

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
  
};

struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t3 worldPosition : POSITION0;
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





ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);


VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    float32_t4x4 WorldViewProjection = mul(gTransformationMatrix.matWorld, mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection));
    
    output.position = mul(input.position, WorldViewProjection);
    output.worldPosition = mul(input.position, gTransformationMatrix.matWorld).xyz;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.WorldInverseTranspose));
    
   
    return output;

}