#include "object3d.hlsli"




ConstantBuffer<TransformationMatrix>gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);




VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
	output.texcoord = input.texcoord;
	output.position = mul(input.position, gTransformationMatrix.WVP);
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.World));
	return output;

}

