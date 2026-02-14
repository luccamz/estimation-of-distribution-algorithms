#include "original_history.hpp"
#include <cmath>
#include <cstdint>

OriginalHistory::OriginalHistory(int n) : min_size(int(std::log(n))) { L = new Node(0, 0, 0, nullptr); }
void OriginalHistory::add(uint8_t b) {
    if (L->H.m < min_size) {
        L->H.m++;
        L->H.h1 += b;
        L->H.h0 += (1 - b);
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
        if (curr->H.m == next->H.m) {
            if (alreadySeenDouble) {
                // merge curr and next
                curr->H.h1 += next->H.h1;
                curr->H.h0 += next->H.h0;
                curr->H.m += next->H.m;
                curr->next = next->next;
                delete next;
                alreadySeenDouble = false; // reset for next pair
                next = curr;               // stay on the current node to check for
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
        Node *next = curr->next;
        delete curr;
        curr = next;
    }
    L = new Node(0, 0, 0, nullptr);
}

std::vector<HistoryTriple> OriginalHistory::get_subsequences() const {
    std::vector<HistoryTriple> subsequences;
    Node *curr = L;
    while (curr != nullptr) {
        subsequences.push_back(curr->H);
        curr = curr->next;
    }
    return subsequences;
}

OriginalHistory::~OriginalHistory() {
    Node *curr = L;
    while (curr != nullptr) {
        Node *next = curr->next;
        delete curr;
        curr = next;
    }
}
