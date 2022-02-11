#pragma once

#include <cstdint>
#include <vector>
#include <random>
#include <algorithm>
#include "Vector2.h"

namespace engine { namespace math {

class Random {
public:
    explicit Random(uint64_t seed = 0);

    void setSeed(uint64_t seed);
    uint64_t getSeed() const;

    int nextInt();
    int nextInt(int min, int max);
    int nextInt(int max);

    float nextFloat();
    float nextFloat(float min, float max);

    double nextDouble();
    double nextDouble(double min, double max);

    bool nextBool();
    bool nextBool(float probability);

    float nextGaussian(float mean = 0.0f, float stddev = 1.0f);
    double nextGaussian(double mean = 0.0, double stddev = 1.0);

    Vector2f nextPointInCircle(float radius = 1.0f);
    Vector2f nextPointOnCircle(float radius = 1.0f);
    Vector2f nextPointInRect(float w, float h);

    template <typename T>
    void shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), mRng);
    }

    template <typename T>
    int weightedSelect(const std::vector<T>& weights) {
        T total = T();
        for (const auto& w : weights) total += w;
        if (total <= T()) return 0;
        T r = static_cast<T>(nextDouble(0.0, static_cast<double>(total)));
        T sum = T();
        for (size_t i = 0; i < weights.size(); ++i) {
            sum += weights[i];
            if (r <= sum) return static_cast<int>(i);
        }
        return static_cast<int>(weights.size()) - 1;
    }

private:
    uint64_t mSeed;
    std::mt19937_64 mRng;
    std::uniform_real_distribution<double> mUniformDist;
};

}} // namespace engine::math
