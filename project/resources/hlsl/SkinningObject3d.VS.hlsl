

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
	float32_t4 weight : WEIGHT0;
	int32_t4 index : INDEX0;
};

struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t3 worldPosition : POSITION0;
};


struct TransformationMatrix {
	float32_t4x4 matWorld;
	float32_t4x4 WorldInverseTranspose;
};



struct ViewProjectionMatrix {
	float32_t4x4 view;
	float32_t4x4 projection;
	float32_t3 worldPosition;
};


struct Well
{
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

ConstantBuffer<TransformationMatrix>gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
StructuredBuffer<Well> gMatrixPalette : register(t0);



struct Skinned{
   float32_t4 position;
   float32_t3 normal;
};


Skinned Skinning(VertexShaderInput input){
Skinned skinned;
    
    skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(input.position, gMatrixPalette[input.index.y].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(input.position, gMatrixPalette[input.index.z].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(input.position, gMatrixPalette[input.index.w].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f;

    skinned.normal = mul(input.normal, (float32_t3x3) gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.y].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.z].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.w].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
    skinned.normal = normalize(skinned.normal);
    
    return skinned;
}

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
    Skinned skinned = Skinning(input);
	float32_t4x4 WorldViewProjection =  mul(gTransformationMatrix.matWorld,mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection));
    
    output.position = mul(skinned.position,WorldViewProjection);
    output.worldPosition = mul(skinned.position, gTransformationMatrix.matWorld).xyz;
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(skinned.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));
    
    //output.texcoord = input.texcoord;
	//output.position = mul(input.position,WorldViewProjection);
	//output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));
	//output.worldPosition = mul(input.position, gTransformationMatrix.matWorld).xyz;
    
	return output;

}

