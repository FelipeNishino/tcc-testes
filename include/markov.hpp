#ifndef TCC_MARKOV_H
#define TCC_MARKOV_H

#include <vector>
#include <random>
#include "utils.hpp"

class Markov {
    public:
        int estado;
        int n_estados;
        std::vector<int> matriz_transicao;
        std::random_device rd;
        Markov(std::vector<int> m):
        estado(c1),n_estados(std::sqrt(m.size())),matriz_transicao(m)
        {}
        // ~Markov();
        void proximo_estado();
        void display();
};

#endif //TCC_MARKOV_H