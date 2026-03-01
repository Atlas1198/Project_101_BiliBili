#pragma once
#include <random>
#include <cstdint>
#include <chrono>

class Random
{
public:
    // 毎回ランダム（起動ごとに違うseed）
    Random()
        : mt(MakeNonDeterministicSeed())
    {
    }

    // 固定seed（デバッグや再現用）
    explicit Random(uint32_t seed)
        : mt(seed)
    {
    }

    float GetFloat(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(mt);
    }

    int GetInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(mt);
    }

    // 途中でseedを変えたい時用（任意）
    void Reseed(uint32_t seed)
    {
        mt.seed(seed);
    }

private:
    static uint32_t MakeNonDeterministicSeed()
    {
        // random_device が使える環境ならそれを優先
        std::random_device rd;

        // rd が「疑似」になってる環境もあるので、時間も混ぜて強化
        const uint64_t t =
            static_cast<uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());

        // いくつか混ぜる（軽いハッシュっぽい混ぜ方）
        uint64_t seed64 = (static_cast<uint64_t>(rd()) << 32) ^ static_cast<uint64_t>(rd());
        seed64 ^= t + 0x9e3779b97f4a7c15ULL + (seed64 << 6) + (seed64 >> 2);

        return static_cast<uint32_t>(seed64 & 0xFFFFFFFFu);
    }

private:
    std::mt19937 mt;
};