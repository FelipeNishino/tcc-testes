#ifndef TCC_MARKOV_H
#define TCC_MARKOV_H

#include <vector>
#include <random>

class Markov {
    public:
        Markov(std::vector<std::vector<double>> m);
        std::vector<std::vector<double>> matriz_transicao;
        std::mt19937 generator;
        int proximo_estado(int atual);
        void display();
};

#endif //TCC_MARKOV_H