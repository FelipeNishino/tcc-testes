#include <iostream>
#include "include/engine.h"

int main() {
    int m_notas[25] = {
        11, 12, 13, 14, 15,
        31, 32, 33, 34, 35,
        21, 22, 23, 24, 25,
        41, 42, 43, 44, 45,
        51, 52, 53, 54, 55
    };

    Engine engine(5, m_notas);
    engine.get_nota();
    engine.get_nota();
    engine.get_nota();

    engine.play();

    // Markov markov(5, m);
    // markov.display();
    // nota(markov);
    // markov.proximo_estado();
    // markov.display();
    // nota(markov);
}
