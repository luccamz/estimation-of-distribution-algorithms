#include "history.hpp"
#include <cstdint>

struct Node {
    HistoryTriple H;
    Node *next;
    Node(int h1_val, int h0_val, int m_val, Node *next_node) : H({h1_val, h0_val, m_val}), next(next_node) {}
};

class OriginalHistory : public History {

  public:
    Node *L;
    const int min_size;

    OriginalHistory(int n);

    void add(uint8_t b) override;

    void wipe() override;

    std::vector<HistoryTriple> get_subsequences() const override;

    ~OriginalHistory();

  private:
    void consolidate();
};
