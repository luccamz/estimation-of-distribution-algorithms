#include "history.hpp"
#include <cstdint>

struct Node {
    int h1, h0, m;
    Node *next;
    Node(int h1_val, int h0_val, int m_val, Node *next_node)
        : h1(h1_val), h0(h0_val), m(m_val), next(next_node) {}
};

class OriginalHistory : public History {

  public:
    Node *L;
    const int min_size;

    OriginalHistory(int n);

    void add(uint8_t b) override;

    void wipe() override;

    ~OriginalHistory();

  private:
    void consolidate();
};
