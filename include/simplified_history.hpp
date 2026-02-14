#include "history.hpp"
#include <cstdint>

class SimplifiedHistory : public History {

  public:
    HistoryTriple H;

    SimplifiedHistory();

    void add(uint8_t b) override;

    void wipe() override;

    std::vector<HistoryTriple> get_subsequences() const override;
};
