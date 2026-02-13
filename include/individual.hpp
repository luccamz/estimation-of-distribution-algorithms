#pragma once
#include <cstdint>
#include <random>
#include <vector>

struct Individual {
    std::vector<uint8_t> bits;
    Individual(int n) : bits(n, 0) {};
};

void mutate(Individual &ind, std::mt19937 &gen);
