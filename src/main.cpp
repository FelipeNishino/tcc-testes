#include <iostream>
#include "include/engine.hpp"
#include "include/device_manager.hpp"
#include <iterator>
#include <vector>
#include "include/libmidifile/MidiFile.h"
#include "include/request_manager.hpp"

void convert() {
    smf::MidiFile midifile;
    midifile.read("/home/nishi/Downloads/midi/bach_846.mid");
       // midifile.write("output.mid");
    std::cout << midifile.status() << std::endl;
    midifile.joinTracks();
    midifile.writeBinascWithComments("/home/nishi/Downloads/midi/bach_846_joined.mid");
}

int main() {
    // int m_notas[25] = {
    //     11, 12, 13, 14, 15,
    //     31, 32, 33, 34, 35, 
    //     21, 22, 23, 24, 25,
    //     41, 42, 43, 44, 45,
    //     51, 52, 53, 54, 55
    // };
    // convert();
    // return 0;
    RequestManager asd;
    // std::vector<BYTE> decodedData = base64_decode(encodedData);

    // std::vector<int> m_notas(25, 1);

    // Engine* engine = Engine::GetInstance(m_notas);
    // DeviceManager dm;
    // std::cout << "device id: " << dm.get_device_id() << std::endl;
    // DeviceManager dm;
    // std::cout << engine->count_notas << std::endl;
    // engine->play();
    // Markov markov(5, m);
    // markov.display();
    // nota(markov);
    // markov.proximo_estado();
    // markov.display();
    // nota(markov);
}
