#include "Particle.hlsli"

RWStructuredBuffer<Particle> gParticle : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b0);

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID ){
    uint32_t particleIndex = DTid.x;
    if (particleIndex < kMaxParticles){
        //alphaが0のparticleは死んでるとみなして更新しない
        if (gParticle[particleIndex].color.a != 0){
            gParticle[particleIndex].translate += gParticle[particleIndex].velocity;
            gParticle[particleIndex].currentTime += gPerFrame.deltaTime;
            float32_t alpha = 1.0f - (gParticle[particleIndex].currentTime / gParticle[particleIndex].lifeTime);
            gParticle[particleIndex].color.a = saturate(alpha);
        }
        
    }
    
}