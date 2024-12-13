#pragma once
#pragma once
#include <random>

/**
 * @class RandomGenerator
 * @brief ランダム数を生成するクラス
 */
class RandomGenerator {
public:
    /// <summary>
    /// ランダムジェネレータを初期化する
    /// </summary>
    static void Initialize();

    /// <summary>
    /// 指定した範囲でランダムな整数を取得する
    /// </summary>
    /// <param name="min">最小値</param>
    /// <param name="max">最大値</param>
    /// <returns>ランダムな整数</returns>
    static int GetRandomInt(int min, int max);

    /// <summary>
    /// 指定した範囲でランダムな浮動小数点数を取得する
    /// </summary>
    /// <param name="min">最小値</param>
    /// <param name="max">最大値</param>
    /// <returns>ランダムな浮動小数点数</returns>
    static float GetRandomFloat(float min, float max);

private:
    // ランダムエンジン（Mersenne Twister）インスタンス
    static std::mt19937 randomEngine_;
};
