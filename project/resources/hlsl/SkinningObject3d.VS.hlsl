

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	float32_t4 weight : WEIGHT0;
	int32_t4 index : INDEX0;
};


ConstantBuffer<TransformationMatrix>gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
StructureBuffer<Well> gMatrixPalette : register(t0);

struct Well{
   float32_t4x4 skeletonSpaceMatrix;
   float32_t4x4 skeletonSpaceInverseTransposeMatrix;
}

struct Skinned{
   float32_t4 position;
   float32_t3 normal;
}


Skinned Skinning(VertexShaderInput input){
Skinned skinned;
    
    skinned.position = mul(float32_t4(input.position,1.0f), gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(float32_t4(input.position,1.0f), gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(float32_t4(input.position,1.0f), gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(float32_t4(input.position,1.0f), gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f;

    skinned.normal = mul(input.normal, (float3x3)gMatrixPalette[input.index.x].skeletonSpaceMInverseTransposeMatrix) * input.weight.x;
    skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.y].skeletonSpaceMInverseTransposeMatrix) * input.weight.y;
    skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.z].skeletonSpaceMInverseTransposeMatrix) * input.weight.z;
    skinned.normal += mul(input.normal, (float3x3)gMatrixPalette[input.index.w].skeletonSpaceMInverseTransposeMatrix) * input.weight.w;
    skinned.normal = normalize(skinned.normal);
    
    return skinned;
}

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
    Skinned skinned = Skinning(input);
	float32_t4x4 WorldViewProjection =  mul(gTransformationMatrix.matWorld,mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection));
    output.position = mul(skinned.position,WorldViewProjection);
    output.worldPosition = mul(skinned.position, gTransformationMatrix.matWorld).xyz;
	output.texcoord = skinned.texcoord;
	output.normal = normalize(mul(skinned.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));
	return output;

}

