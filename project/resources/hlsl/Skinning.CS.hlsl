
struct Well
{
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};
struct Vertex{
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
};
struct VertexInfluence{
    float32_t4 weight;
    int32_t4 index;
};
struct SkinningInformation{
    uint32_t numVertices;
};


//SkinningObject3d.VS.hlslで作ったものと同じPalette
StructuredBuffer<Well> gMatrixPalette : register(t0);
//VertexBufferViewのstream0として利用していた入力頂点
StructuredBuffer<Vertex> gInputVertices : register(t1);
//VertexBufferViewのstream1として利用していたインフルエンス
StructuredBuffer<VertexInfluence> gInfluences : register(t2);
//Skinning計算後の頂点データ
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);
//Skinningに関するちょっとつぃた情報
ConstantBuffer<SkinningInformation> gSkinnningInformation : register(b0);


[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}