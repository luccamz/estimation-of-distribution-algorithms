#pragma once
#include <cstdint>
#include <random>
#include <vector>

struct Individual {
    std::vector<uint8_t> bits;

    double fitness;

    Individual(int n) : bits(n, 0), fitness(-1.0) {};

    Individual(const Individual &) = default;
    Individual &operator=(const Individual &) = default;

    // implements standard bit mutation with O(1) jumps
    Individual mutate(std::mt19937 &gen);
};
