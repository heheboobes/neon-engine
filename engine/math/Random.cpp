#include "Random.h"
#include <cmath>

namespace engine { namespace math {

Random::Random(uint64_t seed)
    : mSeed(seed)
    , mRng(static_cast<std::mt19937_64::result_type>(seed))
    , mUniformDist(0.0, 1.0)
{
}

void Random::setSeed(uint64_t seed) {
    mSeed = seed;
    mRng.seed(static_cast<std::mt19937_64::result_type>(seed));
}

uint64_t Random::getSeed() const {
    return mSeed;
}

int Random::nextInt() {
    return static_cast<int>(mRng());
}

int Random::nextInt(int min, int max) {
    if (min >= max) return min;
    std::uniform_int_distribution<int> dist(min, max - 1);
    return dist(mRng);
}

int Random::nextInt(int max) {
    return nextInt(0, max);
}

float Random::nextFloat() {
    return static_cast<float>(mUniformDist(mRng));
}

float Random::nextFloat(float min, float max) {
    return min + (max - min) * nextFloat();
}

double Random::nextDouble() {
    return mUniformDist(mRng);
}

double Random::nextDouble(double min, double max) {
    return min + (max - min) * nextDouble();
}

bool Random::nextBool() {
    return nextDouble() < 0.5;
}

bool Random::nextBool(float probability) {
    return nextDouble() < static_cast<double>(probability);
}

float Random::nextGaussian(float mean, float stddev) {
    float u1 = nextFloat();
    float u2 = nextFloat();
    float z0 = std::sqrt(-2.0f * std::log(u1 + 1e-10f)) * std::cos(6.2831853f * u2);
    return z0 * stddev + mean;
}

double Random::nextGaussian(double mean, double stddev) {
    double u1 = nextDouble();
    double u2 = nextDouble();
    double z0 = std::sqrt(-2.0 * std::log(u1 + 1e-10)) * std::cos(6.283185307179586 * u2);
    return z0 * stddev + mean;
}

Vector2f Random::nextPointInCircle(float radius) {
    float angle = nextFloat(0.0f, 6.2831853f);
    float r = radius * std::sqrt(nextFloat());
    return Vector2f(r * std::cos(angle), r * std::sin(angle));
}

Vector2f Random::nextPointOnCircle(float radius) {
    float angle = nextFloat(0.0f, 6.2831853f);
    return Vector2f(radius * std::cos(angle), radius * std::sin(angle));
}

Vector2f Random::nextPointInRect(float w, float h) {
    return Vector2f(nextFloat(-w * 0.5f, w * 0.5f), nextFloat(-h * 0.5f, h * 0.5f));
}

}} // namespace engine::math
