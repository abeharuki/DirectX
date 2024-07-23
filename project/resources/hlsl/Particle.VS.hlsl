#include "Particle.hlsli"

StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PreView> gPreView : register(b1);

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	
    Particle particle = gParticle[instanceId];
    float32_t4x4 worldMatrix = gPreView.billboardMatrix;//worldMatrixを作る
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    float32_t4x4 ViewProjection = mul(gPreView.view, gPreView.projection);
	output.position = mul(input.position, mul(worldMatrix,ViewProjection));
	output.texcoord = input.texcoord;
	output.color = particle.color;
	
	return output;
}