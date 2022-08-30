#ifndef TCC_MARKOV_H
#define TCC_MARKOV_H

class Markov {
    public:
        int estado;
        int n_estados;
        int* matriz_transicao;
        Markov(int n, int* m);
        ~Markov();
        void proximo_estado();
        void display();
};

#endif //TCC_MARKOV_H