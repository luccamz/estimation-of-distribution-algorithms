#include "simplified_history.hpp"
#include "history.hpp"
#include <cstdint>

SimplifiedHistory::SimplifiedHistory() : H({0, 0, 0}) {}

void SimplifiedHistory::add(uint8_t b) {
    H.m++;
    b ? H.h1++ : H.h0++;
}

void SimplifiedHistory::wipe() { H = HistoryTriple{0, 0, 0}; }

std::vector<HistoryTriple> SimplifiedHistory::get_subsequences() const { return {H}; }
