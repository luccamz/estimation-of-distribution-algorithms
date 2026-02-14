#include "history.hpp"
#include <cstdint>

class SimplifiedHistory : public History {

  public:
    int h1, h0, m;

    SimplifiedHistory();

    void add(uint8_t b) override;

    void wipe() override;
};
