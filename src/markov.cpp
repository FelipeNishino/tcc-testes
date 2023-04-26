#include <cmath>
#include <iostream>
#include "markov.hpp"
#include "engine.hpp"
#include "logger.hpp"

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
    std::random_device rd;
    // seed value is designed specifically to make initialization
    // parameters of std::mt19937 (instance of std::mersenne_twister_engine<>)
    // different across executions of application
    std::mt19937::result_type seed = rd() ^ (
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
                ).count() +
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
                ).count() );

    // std::mt19937 gen(seed);
    std::uniform_int_distribution<unsigned> distrib(0, m.size());
    // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // generator = std::mt19937(seed);

    // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    matriz_transicao = m;
    // std::mt19937 generator(seed);
    auto p0 = std::chrono::time_point<std::chrono::high_resolution_clock>{};
    std::time_t epoch_time = std::chrono::system_clock::to_time_t(p0);
    generator = std::mt19937(seed);

    Logger::log(Logger::LOG_INFO, "<Markov> Seed: %d", seed);
}

int Markov::proximo_estado(int atual) {
    double val = std::generate_canonical<double,std::numeric_limits<double>::digits>(generator) * 100.0;
    Logger::log(Logger::LOG_DEBUG, "<Markov> Vai gerar o proximo estado");
    double sum{};
    int prox{};
    for (auto prob : matriz_transicao[atual]) {
        sum += prob;
        if (val < sum) break;
        prox++;
    }
    // if (prox == 12) {
    //     prox = proximo_estado(atual);
    // }
    Logger::log(Logger::LOG_DEBUG, "<Markov> Estado selecionado: %d", prox);

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
