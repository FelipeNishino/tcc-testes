#include "include/engine.h"
#include <iostream>

/**
 * Static methods should be defined outside the class.
 */

Engine* Engine::pinstance_{nullptr};
std::mutex Engine::mutex_;

/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value. RU:
 */
Engine *Engine::GetInstance(std::vector<int> m)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr)
    {
        pinstance_ = new Engine(m);
    }
    return pinstance_;
}

int Engine::get_note() {
    int note;
        switch (cadeia_notas.estado) {
        case c1:
            note = c1;
            break;
        case d:
            note = d;
            break;
        case e:
            note = e;
            break;
        case f:
            note = f;
            break;
        case g:
            note = g;
            break;
        case a:
            note = a;
            break;
        case b:
            note = b;
            break;
        case c2:
            note = c2;
            break;
        default:
            std::cout << "Nota nao reconhecida";
    }
    std::cout << std::endl;
    cadeia_notas.proximo_estado();
    count_notas++;
    return note;
}

void Engine::play() {
    try {
	    wrapper.toggle_stream(true);
	}
	catch ( RtAudioError &error ) {
        std::cout << "Erro" << std::endl;
	    error.printMessage();
        return;
	}

	// Block waiting until callback signals done.
	while ( !wrapper.is_done() )
	    stk::Stk::sleep(1000 );
	
	// Shut down the callback and output stream.
	try {
	    wrapper.toggle_stream(false);
	}
	catch ( RtAudioError &error ) {
	    error.printMessage();
        return;
	}
}