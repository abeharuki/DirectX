#include "Skybox.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);


VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float32_t4x4 WorldViewProjection = mul(gTransformationMatrix.matWorld, mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection));
    output.position = mul(input.position, WorldViewProjection).xyww;
    output.texcoord = input.position.xyz;
    return output;
}