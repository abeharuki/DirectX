#pragma once
#include <cstdint>

//CollisionAttribute
const uint32_t kCollisionAttributePlayer = 0b00001;
const uint32_t kCollisionAttributeEnemy = 0b00010;
const uint32_t kCollisionAttributeHealer = 0b00100;
const uint32_t kCollisionAttributeRenju = 0b01000;
const uint32_t kCollisionAttributeTank = 0b10000;


//CollisionMask
const uint32_t kCollisionMaskPlayer = 0b11110;
const uint32_t kCollisionMaskEnemy = 0b11101;
const uint32_t kCollisionMaskHealer = 0b11011;
const uint32_t kCollisionMaskRenju = 0b10111;
const uint32_t kCollisionMaskTank = 0b01111;

//形状
const uint32_t kCollisionPrimitiveSphere = 0b1;
const uint32_t kCollisionPrimitiveAABB = 0b1 << 1;
const uint32_t kCollisionPrimitiveOBB = kCollisionPrimitiveAABB << 1;