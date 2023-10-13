#include "object3d.hlsli"




ConstantBuffer<TransformationMatrix>gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};


VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
	output.texcoord = input.texcoord;
	//output.position = mul(input.position, mul(gTransformationMatrix.WVP, WorldViewProjection));
	//output.normal = normalize(mul(input.normal, (float32_t3x3)WorldViewProjection));
	output.position = mul(input.position, gTransformationMatrix.WVP);
	output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.World));
	return output;

}

