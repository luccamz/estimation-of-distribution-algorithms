#include <cstdint>

class History {
  public:
    History();
    virtual void add(uint8_t x_i); // Adjusts the history with a new bit
    virtual void wipe();           // Resets the history to an empty state
};
