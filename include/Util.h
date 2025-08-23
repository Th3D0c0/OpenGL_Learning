#pragma once 

#include <random>


class RNG
{
public:
    RNG(const RNG&) = delete;
    RNG& operator=(const RNG&) = delete;

    static RNG& Get()
    {
        static RNG instance;
        return instance;
    }

    float RandomFloatInRange(float min, float max)
    {
        return std::uniform_real_distribution<float>{min, max}(m_engine);
    }

    float RandomIntInRange(int min, int max)
    {
        return std::uniform_int_distribution<int>{min, max}(m_engine);
    }


private:
    RNG()
        :m_engine(m_RandomDevice()) {}

    std::random_device m_RandomDevice;  // a seed source for the random number engine
    std::mt19937 m_engine;
};

