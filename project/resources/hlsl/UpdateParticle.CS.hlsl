#include "Particle.hlsli"

struct AccelerationField
{
    float32_t3 acceleration; //加速度
    float32_t3 translate; //位置
    float32_t3 min; //最小範囲
    float32_t3 max; //最大範囲
};

struct GravityField
{
    float32_t3 translate; //位置
    float32_t3 min; //最小範囲
    float32_t3 max; //最大範囲
    float32_t strength; //重力の強さ
    float32_t stopDistance; //動きを止める中心点からの距離
};


RWStructuredBuffer<Particle> gParticle : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);
StructuredBuffer<AccelerationField> gAccelerationField : register(t0);
StructuredBuffer<GravityField> gGravityField : register(t1);



[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    uint32_t particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        //alphaが0のparticleは死んでるとみなして更新しない
        if (gParticle[particleIndex].color.a != 0)
        {
            
            AccelerationField field = gAccelerationField[0];
            if (gParticle[particleIndex].translate.x >= field.translate.x + field.min.x &&
                gParticle[particleIndex].translate.y >= field.translate.y + field.min.y &&
                gParticle[particleIndex].translate.z >= field.translate.z + field.min.z &&
                gParticle[particleIndex].translate.x <= field.translate.x + field.max.x &&
                gParticle[particleIndex].translate.y <= field.translate.y + field.max.y &&
                gParticle[particleIndex].translate.z <= field.translate.z + field.max.z)
            {
                gParticle[particleIndex].velocity += field.acceleration * gPerFrame.deltaTime;
            }
            
            
            GravityField gravityField = gGravityField[0];
            if (gParticle[particleIndex].translate.x >= gravityField.translate.x + gravityField.min.x &&
                gParticle[particleIndex].translate.y >= gravityField.translate.y + gravityField.min.y &&
                gParticle[particleIndex].translate.z >= gravityField.translate.z + gravityField.min.z &&
                gParticle[particleIndex].translate.x <= gravityField.translate.x + gravityField.max.x &&
                gParticle[particleIndex].translate.y <= gravityField.translate.y + gravityField.max.y &&
                gParticle[particleIndex].translate.z <= gravityField.translate.z + gravityField.max.z)
            {
                float32_t3 direction = gravityField.translate - gParticle[particleIndex].translate;
                float32_t distance = length(direction);
                if (distance > gravityField.stopDistance)
                {
                    gParticle[particleIndex].velocity += normalize(direction) * gravityField.strength * gPerFrame.deltaTime;
                }
                else
                {
                    gParticle[particleIndex].velocity *= 0.9f;
                }
            }
            
            
            gParticle[particleIndex].translate += gParticle[particleIndex].velocity;  
            gParticle[particleIndex].currentTime += gPerFrame.deltaTime;
            
            
             //イージング係数を計算
            float32_t easingParameter = saturate(gParticle[particleIndex].currentTime / gParticle[particleIndex].lifeTime);
            //アルファの更新
            gParticle[particleIndex].color.a = saturate(lerp(gParticle[particleIndex].initialAlpha, gParticle[particleIndex].endAlpha, easingParameter));
            //スケールの更新
            gParticle[particleIndex].scale = lerp(gParticle[particleIndex].initialScale, gParticle[particleIndex].endScale, easingParameter);
        
        }
            
        if (gParticle[particleIndex].currentTime >= gParticle[particleIndex].lifeTime)
        {
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