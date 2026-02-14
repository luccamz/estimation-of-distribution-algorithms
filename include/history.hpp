#pragma once
#include <cstdint>

class History {
  public:
    virtual ~History() = default;
    virtual void add(uint8_t x_i) = 0; // Adjusts the history with a new bit
    virtual void wipe() = 0;           // Resets the history to an empty state
};
