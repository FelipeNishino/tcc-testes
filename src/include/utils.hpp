#ifndef TCC_UTILS_H
#define TCC_UTILS_H

enum notas {
    c1, d, e, f, g, a, b, c2
}; 

// static int NOTE_TO_FREQ[] = {264, 297, 330, 352, 396, 440, 495, 528};
static const int NOTE_TO_MIDI_KEY[] = {60, 62, 64, 65, 67, 69, 71, 72};

#endif //TCC_UTILS_H