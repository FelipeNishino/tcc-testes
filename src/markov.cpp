#include <cmath>
#include <iostream>
#include "markov.hpp"
#include "engine.hpp"
#include "utils.hpp"

// Markov::Markov(std::vector<int> m) {
//     estado = c1;
//     n_estados = std::sqrt(m.size());
//     // int i, j;
//     std::vector<int> matriz_transicao(m);

//     // for (i = 0; i < n_estados; i++) {    
//     //     for (j = 0; j < n_estados; j++)
//     //         matriz_transicao[i * n_estados + j] = m[i * n_estados + j];
//     // }
// }

Markov::Markov(std::vector<std::vector<double>> m) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    matriz_transicao = m;
    std::default_random_engine generator(seed);
}

int Markov::proximo_estado(int atual) {
    double val = std::generate_canonical<double,std::numeric_limits<double>::digits>(generator) * 100.0;
    double sum{};
    int prox{};
    for (auto prob : matriz_transicao[atual]) {
        sum += prob;
        if (val < sum) break;
        prox++;
    }
    if (prox == 12) {
        prox = proximo_estado(atual);
    }
    return prox;
}

void Markov::display() {
    int i, j;

    std::cout << "Transicoes: "  << std::endl;
    for (i = 0; i < matriz_transicao.size(); i++) {
        for (j = 0; j < matriz_transicao.size(); j++)
            std::cout << " | " << matriz_transicao[i][j];
        std::cout << " |" << std::endl;
    }
    std::cout << std::endl;
}
