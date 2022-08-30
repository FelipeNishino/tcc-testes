#include "include/engine.h"
#include <iostream>

void Engine::get_nota() {
        switch (cadeia_notas.estado) {
        case c:
            std::cout << "do";
            break;
        case d:
            std::cout << "re";
            break;
        case e:
            std::cout << "mi";
            break;
        case f:
            std::cout << "fa";
            break;
        case g:
            std::cout << "so";
            break;
        case a:
            std::cout << "la";
            break;
        case b:
            std::cout << "si";
            break;
    }
    std::cout << std::endl;
    cadeia_notas.proximo_estado();
}