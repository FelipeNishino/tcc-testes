#include "include/markov.h"
#include <random>
#include <iostream>

Markov::Markov(int n, int* m) {
    estado = 0;
    n_estados = n;
    int i, j;
    int n_total = n*n;
    matriz_transicao = (int*) malloc(n_total * sizeof(int));

    for (i = 0; i < n; i++) {    
        for (j = 0; j < n; j++)
            matriz_transicao[i * n + j] = m[i * n + j];
    }
}

Markov::~Markov() {
    // free(matriz_transicao);
}

void Markov::proximo_estado() {
    std::random_device rd;
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
