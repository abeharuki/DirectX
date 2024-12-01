#pragma once
#include <cstdint>

//CollisionAttribute
const uint32_t kCollisionAttributePlayer = 0b00001;
const uint32_t kCollisionAttributeEnemy = 0b00010;
const uint32_t kCollisionAttributeHealer = 0b00100;
const uint32_t kCollisionAttributeRenju = 0b01000;
const uint32_t kCollisionAttributeTank = 0b10000;
const uint32_t kCollisionAttributeLoderWall = 0b100000;
const uint32_t kCollisionAttributeEnemyAttack = 0b100010;
const uint32_t kCollisionAttributeRenjuAttack = 0b101000;
const uint32_t kCollisionAttributeHenchman = 0b1000000;

//CollisionMask /*CollisionAttributeで設定した1の部分と同じ部分を、１にすると当たり判定をとる*/
const uint32_t kCollisionMaskPlayer = 0b1111110;
const uint32_t kCollisionMaskEnemy = 0b1111101;
const uint32_t kCollisionMaskHealer = 0b1111011;
const uint32_t kCollisionMaskRenju = 0b1110111;
const uint32_t kCollisionMaskTank = 0b1101111;
const uint32_t kCollisionMaskWall = 0b0011111;
const uint32_t kCollisionMaskEnemyAttack = 0b0011101;
const uint32_t kCollisionMaskRenjuAttack = 0b0000010;
const uint32_t kCollisionMaskHenchman = 0b0011111;

//形状
const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = 0b1 << 1;
const uint32_t kCollisionPrimitiveOBB = kCollisionPrimitiveAABB << 1;