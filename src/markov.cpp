#include "include/markov.h"
#include <cmath>
#include <random>
#include <iostream>
#include <vector>
#include "include/utils.h"

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

void Markov::proximo_estado() {
    estado = rd() % n_estados;
}

void Markov::display() {
    int i, j;

    std::cout << "Estados: " << n_estados << std::endl;
    std::cout << "Estado atual: " << estado << std::endl << std::endl;
    std::cout << "Transicoes: "  << std::endl;
    for (i = 0; i < n_estados; i++) {
        for (j = 0; j < n_estados; j++)
            std::cout << " | " << matriz_transicao[i * n_estados + j];
        std::cout << " |" << std::endl;
    }
    std::cout << std::endl;
}
