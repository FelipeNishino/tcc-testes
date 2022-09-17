#include "include/engine.h"
#include <iostream>

void Engine::get_nota() {
        switch (cadeia_notas.estado) {
        case c1:
            std::cout << "do1";
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
            std::cout << "sol";
            break;
        case a:
            std::cout << "la";
            break;
        case b:
            std::cout << "si";
            break;
        case c2:
            std::cout << "do2";
            break;
    }
    std::cout << std::endl;
    cadeia_notas.proximo_estado();
}

void Engine::play() {
    // while(true) {
    //     try {
	//         dac.startStream();
	//     }
	//     catch ( RtAudioError &error ) {
	//         error.printMessage();
	//     goto cleanup;
	// }

	// // Block waiting until callback signals done.
	// while ( !data.done )
	// Stk::sleep( 100 );
	
	// // Shut down the callback and output stream.
	// try {
	// dac.closeStream();
	// }
	// catch ( RtAudioError &error ) {
	// error.printMessage();
	// }

    // }
}