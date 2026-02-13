#pragma once
#include <cstdint>
#include <random>
#include <vector>

struct Individual {
    std::vector<uint8_t> bits;
    double fitness = 0.0;

    Individual(int n);
};

void mutate(Individual &ind, std::mt19937 &gen);
