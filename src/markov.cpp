#include <cmath>
#include <iostream>
#include "markov.hpp"
#include "engine.hpp"
#include "logger.hpp"

Markov::Markov(std::vector<std::vector<double>> m) {
    std::random_device rd;

    std::mt19937::result_type seed = rd() ^ (
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
                ).count() +
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
                ).count() );

    std::uniform_int_distribution<unsigned> distrib(0, m.size());

    matriz_transicao = m;
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
