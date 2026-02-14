#include "original_history.hpp"
#include <cmath>
#include <cstdint>

OriginalHistory::OriginalHistory(int n) : min_size(int(std::log(n))) {
    L = new Node(0, 0, 0, nullptr);
}
void OriginalHistory::add(uint8_t b) {
    if (L->m < min_size) {
        L->m++;
        L->h1 += b;
        L->h0 += (1 - b);
    } else {
        L = new Node(b, 1 - b, 1, L);
        this->consolidate();
    }
}

void OriginalHistory::consolidate() {
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

void OriginalHistory::wipe() {
    Node *curr = L;
    while (curr != nullptr) {
        curr->h1 = 0;
        curr->h0 = 0;
        curr->m = 0;
        curr = curr->next;
    }
}

OriginalHistory::~OriginalHistory() {
    Node *curr = L;
    while (curr != nullptr) {
        Node *next = curr->next;
        delete curr;
        curr = next;
    }
}
