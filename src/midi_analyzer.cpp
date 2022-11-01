#include "midi_analyzer.hpp"
#include "libmidifile/MidiEvent.h"
#include "libmidifile/MidiFile.h"
#include <array>
#include <iostream>
/*
NOTE ON/OFF
DELTA? EVENTCODE NOTA VELOCITY 
' = DECIMAL
*/

void MidiAnalyzer::assert_containing_dir_path() {
    if (containing_dir.empty()) throw midi_containing_dir_empty(); 
}

nlohmann::json MidiAnalyzer::analyze(std::vector<std::string> midi_list) {
    assert_containing_dir_path();
    smf::MidiEvent last_note_on;
    smf::MidiFile midifile;
    std::array<std::array<int, 13>, 13> note_occurrence;
    
    for (auto midi_name : midi_list) {
        midifile.read(midi_name);
        if (!midifile.status()) {
            std::cout << "Error opening file " << midi_name << '\n';
            continue;
        }

        for (int i = 0; i < midifile.getTrackCount(); i++) {
            for (int j = 0; j < midifile[i].getEventCount(); j++) {
                if (midifile[i][j].isNoteOn()) {
                    if (last_note_on.track == -1) last_note_on = midifile[i][j];
                    else {
                        note_occurrence[last_note_on.getKeyNumber() % 12][midifile[i][j].getKeyNumber() % 12]++;
                        last_note_on = midifile[i][j];
                    }
                } else if (midifile[i][j].isNoteOff()) {
                    
                }
            }
        }
    }
}