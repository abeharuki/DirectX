#include "Particle.hlsli"

RWStructuredBuffer<Particle> gParticle : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);


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
        }else{
            //Sxaleに0を入れておいてVertexShadeer出力で棄却されるようにする
            gParticle[particleIndex].scale = float32_t3(0.0f, 0.0f, 0.0f);
            int32_t freeListIndex;
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
            //最新のFreeListIndexの場所に死んだParticleのIndexを設定
            if ((freeListIndex + 1) < kMaxParticles)
            {
                gFreeList[freeListIndex + 1] = particleIndex;
            }
            else
            {
                //安全策。ここに来ることはない来たら何かが間違ってる
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);

            }
            
        }
        
    }
    
}