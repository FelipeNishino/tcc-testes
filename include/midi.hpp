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

    enum Notes {
        c1, cs, d, ds, e, f, fs, g, gs, a, as, b
    };
    static const int NOTE_TO_MIDI_KEY[] = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    static void read_midi(smf::MidiFile* midifile, std::string path) {
        midifile->read(path);
        if (!midifile->status()) throw unsuccesful_midi_read(path);
    }
    // static int NOTE_TO_FREQ[] = {264, 297, 330, 352, 396, 440, 495, 528};
    // static const int NOTE_TO_MIDI_KEY[] = {60, 62, 64, 65, 67, 69, 71, 72};
    // static const int NOTE_TO_MIDI_KEY[] = {48, 50, 52, 53, 55, 57, 59, 60};
    // static const int NOTE_TO_MIDI_KEY[] = {48, 49, 50, 51, 52, 53, 55, 56, 57, 58, 59, 60};
}

#endif //TCC_MIDI_H