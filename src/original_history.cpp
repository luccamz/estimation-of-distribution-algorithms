#include "history.hpp"
#include <cmath>
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

    OriginalHistory(int n) : min_size(int(std::log(n))) {
        L = new Node(0, 0, 0, nullptr);
    }
    void add(uint8_t b) {
        if (L->m < min_size) {
            L->m++;
            L->h1 += b;
            L->h0 += (1 - b);
        } else {
            L = new Node(b, 1 - b, 1, L);
            this->consolidate();
        }
    }

  private:
    void consolidate() {
        // algorithm 2
        Node *curr = L;
        bool alreadySeenDouble = false;
        while (curr->next != nullptr) {
            auto next = curr->next;
            if (curr->m == next->m) {
                if (alreadySeenDouble) {
                    // merge curr and next
                    curr->h1 += next->h1;
                    curr->h0 += next->h0;
                    curr->m += next->m;
                    curr->next = next->next;
                    delete next;
                    alreadySeenDouble = false; // reset for next pair
                    next = curr; // stay on the current node to check for
                                 // further merges
                } else {
                    alreadySeenDouble = true;
                }
            }
            curr = next;
        }
    }
};
