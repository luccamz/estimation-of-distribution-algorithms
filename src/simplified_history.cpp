#include "history.hpp"
#include <cstdint>

class SimplifiedHistory : public History {

  public:
    int h1, h0, m;

    SimplifiedHistory() : h1(0), h0(0), m(0) {}

    void add(uint8_t b) {
        m++;
        b ? h1++ : h0++;
    }

    void wipe() {
        h1 = 0;
        h0 = 0;
        m = 0;
    }
};
