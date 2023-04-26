#ifndef TCC_MIDI_H
#define TCC_MIDI_H

#include <exception>
#include <string>
#include "libmidifile/MidiFile.hpp"

namespace Midi {
    struct unsuccesful_midi_read : public std::exception {
        std::string err_msg;
        public:
        unsuccesful_midi_read(std::string path)
        : err_msg(std::string("Couldn't load midi file with path ") + path)
        {}
        const char * what () const throw () {
            return err_msg.c_str();
        }
    };

    static void read_midi(smf::MidiFile* midifile, std::string path) {
        midifile->read(path);
        if (!midifile->status()) throw unsuccesful_midi_read(path);
    }
}

#endif //TCC_MIDI_H