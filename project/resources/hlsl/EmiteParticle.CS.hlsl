#include "Particle.hlsli"

struct EmitterSphere
{
    float32_t3 translate; //位置
    float32_t radius; //射出半径
    uint32_t count; //射出数
    float32_t frequency; //射出間隔
    float32_t frequencyTime; //射出間隔調整時間
    uint32_t emit; //射出許可
};



RWStructuredBuffer<Particle> gParticle : register(u0);
ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);
RWStructuredBuffer<int32_t> gFreeCounter : register(u1);

float3 rand3dTo3d(float3 value, float3 minRange, float3 maxRange)
{
    // 各成分を個別に扱い、独立したランダム値を生成する
    float3 smallValue = sin(value);
    float3 dotDir = float3(12.9898, 78.233, 37.719);
    
    float randomX = dot(smallValue, dotDir);
    randomX = frac(sin(randomX) * 143758.5453);

    // 別の方向ベクトルを使って、次の成分のランダム値を生成
    dotDir = float3(39.346, 11.135, 97.693);
    float randomY = dot(smallValue, dotDir);
    randomY = frac(sin(randomY) * 428758.5431);

    // さらに別の方向ベクトルを使って、次の成分のランダム値を生成
    dotDir = float3(27.034, 87.491, 13.874);
    float randomZ = dot(smallValue, dotDir);
    randomZ = frac(sin(randomZ) * 234758.5465);

    // 0.0から1.0の範囲にあるランダム値を、指定された範囲にスケーリングする
    float3 randomValue = float3(randomX, randomY, randomZ);
    randomValue = lerp(minRange, maxRange, randomValue);

    return randomValue;
}

float rand3dTo1d(float3 value, float minRange, float maxRange)
{
    // 各成分を個別に扱い、独立したランダム値を生成する
    float3 smallValue = sin(value);
    float3 dotDir = float3(12.9898, 78.233, 37.719);
    
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);

    
    return lerp(minRange, maxRange, random);
}

class RandomGenerator
{
    float32_t3 speed;
    float32_t3 Generate3d(float3 minRange, float3 maxRange)
    {
        speed = rand3dTo3d(speed,minRange,maxRange);
        return speed;
    }
    
    float32_t Generate1d(float minRange, float maxRange)
    {
        float32_t result = rand3dTo1d(speed, minRange, maxRange);
        speed.x = result;
        return result;
    }
    
};

[numthreads(1, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID ){
    if (gEmitter.emit != 0){//射出許可の有無
        //初期化
        RandomGenerator generator;
        generator.speed = (DTid + gPerFrame.time) * gPerFrame.time;
        for (uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex){
            int32_t particleIndex;
            InterlockedAdd(gFreeCounter[0], 1, particleIndex);
            if (particleIndex < kMaxParticles){
                 //カウント分Particle
                gParticle[particleIndex].scale = generator.Generate3d(float3(0, 0, 0), float3(1, 1, 1));
                gParticle[particleIndex].translate = generator.Generate3d(float3(0, 0, 0), float3(1, 1, 1));
                gParticle[particleIndex].color.rgb = generator.Generate3d(float3(0, 0, 0), float3(1, 1, 1));
                gParticle[particleIndex].color.a = 1.0f;
            
                gParticle[particleIndex].lifeTime = generator.Generate1d(0,3); //0から3の間のランダム値
                gParticle[particleIndex].velocity = generator.Generate3d(float3(-1, -1, -1), float3(1, 1, 1)); //速度ベクトルをランダムに設定
                gParticle[particleIndex].currentTime = 0.0f; // 初期化時点での経過時間は0
            }
            
           
        }

    }

}