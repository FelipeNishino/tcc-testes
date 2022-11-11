#ifndef TCC_UTILS_H
#define TCC_UTILS_H

#include <string>
#include <array>

enum notas {
    c1, cs, d, ds, e, f, fs, g, gs, a, as, b
}; 

// static int NOTE_TO_FREQ[] = {264, 297, 330, 352, 396, 440, 495, 528};
// static const int NOTE_TO_MIDI_KEY[] = {60, 62, 64, 65, 67, 69, 71, 72};
// static const int NOTE_TO_MIDI_KEY[] = {48, 50, 52, 53, 55, 57, 59, 60};
static const int NOTE_TO_MIDI_KEY[] = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
// static const int NOTE_TO_MIDI_KEY[] = {48, 49, 50, 51, 52, 53, 55, 56, 57, 58, 59, 60};
static const std::array<std::string, 4> EMO_TO_STR = {"happy", "sad", "angry", "relaxed"};

#endif //TCC_UTILS_H