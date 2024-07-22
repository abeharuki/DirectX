#include "Particle.hlsli"

//StructuredBuffer<ParticleForGPU> gParticle : register(t0);
//ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);
StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PreView> gPreView : register(b1);

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID) {
	VertexShaderOutput output;
	//float32_t4x4 WorldViewProjection = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
	//output.position = mul(input.position, mul(gParticle[instanceId].World, WorldViewProjection));
	//output.texcoord = input.texcoord;
	//output.normal = normalize(mul(input.normal, (float32_t3x3)gParticle[instanceId].World));
	//output.color = gParticle[instanceId].color;
	
    Particle particle = gParticle[instanceId];
    float32_t4x4 worldMatrix = gPreView.billboardMatrix;//worldMatrixを作る
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz *= particle.translate;
    float32_t4x4 ViewProjection = mul(gPreView.view, gPreView.projection);
	output.position = mul(input.position, mul(worldMatrix,ViewProjection));
	output.texcoord = input.texcoord;
	output.color = particle.color;
	
	return output;
}