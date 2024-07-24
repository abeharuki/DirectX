#include "Particle.hlsli"

struct Range
{
    float32_t3 min;
    float32_t3 max;
};

struct EmitterSphere
{
    float32_t3 translate; //位置
    float32_t radius; //射出半径
    uint32_t count; //射出数
    float32_t frequency; //射出間隔
    float32_t frequencyTime; //射出間隔調整時間
    uint32_t emit; //射出許可]
    Range scaleRange;
    Range translateRange;
    Range colorRange;
    Range velocityRange; 
};



RWStructuredBuffer<Particle> gParticle : register(u0);
ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);

float32_t rand3dTo1d(float32_t3 value, float32_t3 dotDir = float32_t3(12.9898, 78.233, 37.719))
{
    float32_t3 smallValue = sin(value);
    float32_t random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}

float32_t3 rand3dTo3d(float32_t3 value)
{
    return float32_t3(
        rand3dTo1d(value, float32_t3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, float32_t3(39.346, 11.135, 83.155)),
        rand3dTo1d(value, float32_t3(73.156, 52.235, 09.151))
    );
}

class RandomGenerator
{
    float32_t3 speed;
    float32_t3 Generate3d(float3 minRange, float3 maxRange)
    {
        speed = rand3dTo3d(speed);
        return lerp(minRange,maxRange,speed);
    }
    
    float32_t Generate1d(float minRange, float maxRange)
    {
        float32_t result = rand3dTo1d(speed);
        speed.x = result;
        return lerp(minRange, maxRange,result);
    }
    
};

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID ){
    if (gEmitter.emit != 0){//射出許可の有無
        //初期化
        RandomGenerator generator;
        generator.speed = (DTid + gPerFrame.time) * gPerFrame.time;
        for (uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex){
            int32_t freeListIndex;
            //FreeListのIndexを1つ前に設定し,現在のIndexを取得
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            if (0 <= freeListIndex && freeListIndex < kMaxParticles)
            {
                uint32_t particleIndex = gFreeList[freeListIndex];
                gParticle[particleIndex].scale = generator.Generate3d(float3(gEmitter.scaleRange.min), float3(gEmitter.scaleRange.max));
                gParticle[particleIndex].translate = gEmitter.translate + generator.Generate3d(float3(gEmitter.translateRange.min), float3(gEmitter.translateRange.max));
                gParticle[particleIndex].color.rgb = generator.Generate3d(float3(gEmitter.colorRange.min), float3(gEmitter.colorRange.max));
                gParticle[particleIndex].color.a = 1.0f;
            
                gParticle[particleIndex].lifeTime = generator.Generate1d(3, 3); //0から3の間のランダム値
                gParticle[particleIndex].velocity = generator.Generate3d(float3(gEmitter.velocityRange.min), float3(gEmitter.velocityRange.max)); //速度ベクトルをランダムに設定
                gParticle[particleIndex].currentTime = 0.0f; // 初期化時点での経過時間は0
            }else{
             //発生させられなかったので、減らしてしまった分元に戻す。
                InterlockedAdd(gFreeListIndex[0], 1);
                
              break;
            }   
        }

    }

}