#pragma once
#pragma once
#include <random>

/**
 * @class RandomGenerator
 * @brief ランダム数を生成するクラス
 */
class RandomGenerator {
public:
	static void Initialize();

	static int GetRandomInt(int min, int max);

	static float GetRandomFloat(float min, float max);

private:
	static std::mt19937 randomEngine_;
};