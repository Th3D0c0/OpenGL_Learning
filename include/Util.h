#pragma once 

#include <random>

int RandomNumInRange(int min, int max)
{
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(min, max);

    return distrib(gen);
}