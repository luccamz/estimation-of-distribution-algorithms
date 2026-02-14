#include "simplified_history.hpp"
#include <cstdint>

SimplifiedHistory::SimplifiedHistory() : h1(0), h0(0), m(0) {}

void SimplifiedHistory::add(uint8_t b) {
    m++;
    b ? h1++ : h0++;
}

void SimplifiedHistory::wipe() {
    h1 = 0;
    h0 = 0;
    m = 0;
}
