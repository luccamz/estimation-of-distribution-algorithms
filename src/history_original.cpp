#include "history.hpp"
#include "consolidate.cpp"

struct Node{
    int h1, h0, m;
    Node* next;
    Node(int h1_val, int h0_val, int m_val, Node* next_node) : h1(h1_val), h0(h0_val), m(m_val), next(next_node) {}
};

class OriginalHistory : public History {

    public:

        Node* head;
        int min_size;

        OriginalHistory(int n_size) {
            head = nullptr;
            min_size = int(std::log(n_size));
        }

        void adjustHead(uint8_t x_i)  {
            if (head->m >= min_size) {
                head = new Node(0, 0, 1, head);
                consolidate(head);
                bool aux = x_i == 1 ? head->h1++ : head->h0++;
                return;
            }
            head->m++;
            bool aux = x_i == 1 ? head->h1++ : head->h0++;
        }

        void consolidate(Node* head){
            Node* curr = head;
            if(curr == nullptr) return;
            Node* next = curr->next;
            bool alreadySeenDouble = false;

            while(next != nullptr){
                if(curr->m == next->m){
                    if(alreadySeenDouble){
                        curr->h1 += next->h1;
                        curr->h0 += next->h0;
                        curr->m += next->m;
                        curr->next = next->next;

                        delete next;
                        next = curr->next;
                        alreadySeenDouble = false;

                    }else {
                        alreadySeenDouble = true;
                        curr = next
                        next = next->next;
                    }
                } else {
                    alreadySeenDouble = false;
                    curr = next;
                    next = next->next;
                }
            }

        }



        virtual ~OriginalHistory() {};

};