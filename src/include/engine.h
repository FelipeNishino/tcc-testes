#ifndef TCC_ENGINE_H
#define TCC_ENGINE_H

#include "markov.h"

enum notas {
    c, d, e, f, g, a, b
}; 

class Engine {
    public:
        Markov cadeia_notas;
        Engine(int n, int* m_notas)
        :
        cadeia_notas(n, m_notas)
        {}
        
        void get_nota();
};

#endif //TCC_ENGINE_H