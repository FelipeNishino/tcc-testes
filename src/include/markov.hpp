#ifndef TCC_MARKOV_H
#define TCC_MARKOV_H

#include <vector>

class Markov {
    public:
        int estado;
        int n_estados;
        std::vector<int> matriz_transicao;
        Markov(std::vector<int> m);
        ~Markov();
        void proximo_estado();
        void display();
};

#endif //TCC_MARKOV_H