#pragma once
#include <cstdint>
#include <vector>

struct HistoryTriple {
    int h1;
    int h0;
    int m;
};

class History {
  public:
    virtual ~History() = default;
    virtual void add(uint8_t x_i) = 0; // Adjusts the history with a new bit
    virtual void wipe() = 0;           // Resets the history to an empty state
    // Retrieves all subsequences of the history as triples (h1, h0, m)
    virtual std::vector<HistoryTriple> get_subsequences() const = 0;
};
