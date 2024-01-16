#include "Sprite.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.texcoord = input.texcoord;
	output.position = mul(input.position, gTransformationMatrix.WVP);
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.World));
	return output;
}
