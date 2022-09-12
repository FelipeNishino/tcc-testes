#ifndef TCC_ENGINE_H
#define TCC_ENGINE_H

#include "markov.h"
#include "stk_wrapper.h"

enum notas {
    c1 = 264, d = 297, e = 330, f = 352, g = 396, a = 440, b = 495, c2 = 528
}; 

class Engine {
    public:
        Markov cadeia_notas;
        StkWrapper wrapper;
        Engine(int n, int* m_notas)
        :
        cadeia_notas(n, m_notas)
        {}
        void get_nota();
        void play();
};

#endif //TCC_ENGINE_H