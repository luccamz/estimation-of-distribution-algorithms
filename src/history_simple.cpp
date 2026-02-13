#include "history.hpp"

class SimpleHistory : public History {

    public:

        int h1, h0, m;

        SimpleHistory() : h1(0), h0(0), m(0) {}

        void increment(bool b) {
            m++;
            b = true ? h1++ : h0++;
        }

        virtual ~SimpleHistory() {};

};