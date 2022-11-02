#include "midi_analyzer.hpp"
#include "libmidifile/MidiEvent.h"
#include "libmidifile/MidiFile.h"
#include <array>
#include <iostream>
#include <list>
#include <algorithm>
/*
NOTE ON/OFF
DELTA? EVENTCODE NOTA VELOCITY 
' = DECIMAL
*/

void MidiAnalyzer::assert_containing_dir_path() {
    if (containing_dir.empty()) throw midi_containing_dir_empty(); 
}

EventType MidiAnalyzer::get_event_type(smf::MidiEvent ev) {
    if (ev.isNoteOn())
        return EventType::note_on;
    if (ev.isNoteOff())
        return EventType::note_off;
    if (ev.isTimeSignature())
        return EventType::time_signature;
    if (ev.isKeySignature())
        return EventType::key_signature;
    if (ev.isTempo())
        return EventType::tempo;
    
    return EventType::other;
}
void MidiAnalyzer::analyze(std::string midi_name) {
    smf::MidiEvent last_note_on;
    NoteOff last_note_off;
    smf::MidiFile midifile;
    CurrentNote curr_note;
    LastNoteEvent last_note_event;
    std::_List_iterator<int> find_it;
    bool first_note_on = true;
    std::array<std::array<int, 13>, 13> note_occurrence{0};
    std::list<int> chord_list;
    int j = 1;

    midifile.read(containing_dir + midi_name);
    if (!midifile.status()) {
        std::cout << "Error opening file " << midi_name << '\n';
        return;
    }

    if (midifile.isAbsoluteTicks()) {
        midifile.deltaTicks();
    }
    std::cout << "Tracks: " << midifile.getTrackCount() << '\n';
    for (int i = 0; i < midifile.getTrackCount(); i++) {
        std::cout << "Event count: " << midifile[i].getEventCount() << '\n';
        j = 0;
        first_note_on = true;
        while (j < midifile[i].getEventCount()) {
            curr_note.note = midifile[i][j];
            curr_note.key = midifile[i][j].getKeyNumber();
            
            switch (get_event_type(midifile[i][j])) {
                case note_on:
                    std::cout << "Note on\n";
                    if (first_note_on) {
                        std::cout << "First note\n";
                        first_note_on = false;
                        last_note_on = curr_note.note;
                        j++;
                        last_note_event.note = midifile[i][j];
                        last_note_event.type = on;
                        continue;
                    }
                    // if (chord_list.empty()) chord_list.push_back(curr_note.key);
                    if (curr_note.note.tick == 0 && last_note_event.type == on) {
                        if (chord_list.empty()) chord_list.push_back(last_note_on.getKeyNumber());
                        chord_list.push_back(curr_note.key);
                    }
                    else {
                        if (chord_list.size() == 1 && chord_list.front() == last_note_on.getKeyNumber()) chord_list.clear();
                        last_note_on = curr_note.note;
                    }
                    last_note_event.note = midifile[i][j];
                    last_note_event.type = on;
                    break;
                case note_off:
                    std::cout << "Note off\n";
                    find_it = std::find(chord_list.begin(), chord_list.end(), curr_note.key);
                    if (find_it != chord_list.end()) {
                        std::cout << "Nota acorde\n";
                        chord_list.erase(find_it);
                        if (chord_list.empty()) {
                            std::cout << "Finalizou um acorde\n";
                            if (last_note_off.set) {
                                std::cout << "Último note off: nota " << last_note_off.note.getKeyNumber() % 12 << ", Tipo: " << (last_note_off.type == chord ? "Acorde\n": "Nota\n");
                                note_occurrence[(last_note_off.type == chord ? 12 : last_note_off.note.getKeyNumber() % 12)][12]++;
                            }
                            last_note_off.type = chord;
                            last_note_off.set = true;
                        }
                    }
                    else {
                        std::cout << "Nota normal\n";
                        if (last_note_off.set) {
                            std::cout << "Tem nota anterior: " << last_note_off.note.getKeyNumber() % 12 << ", nota atual: " << curr_note.key % 12 << '\n';
                            note_occurrence[(last_note_off.type == chord ? 12 : last_note_off.note.getKeyNumber() % 12)][curr_note.key % 12]++;                            
                        }
                        else {
                            std::cout << "Não Tem nota anterior\n";
                        }
                        last_note_off.type = note;
                        last_note_off.set = true;
                        last_note_off.note = curr_note.note;
                    }
                    last_note_event.note = midifile[i][j];
                    last_note_event.type = off;
                    break;
                case time_signature:
                    break;
                case key_signature:
                    break;
                case tempo:
                    break;
                case other:
                    std::cout << "Other...\n";
                    break;
            }
            j++;
        }
        std::array<std::string, 13> labels={ "C", "c", "D", "d", "E", "F", "f", "G", "g", "A", "a", "B", "X"};
        for (auto note : {" ", "C", "c", "D", "d", "E", "F", "f", "G", "g", "A", "a", "B", "X"}) {
            std::cout << note << " ";
        }
        std::cout << "\n";
        int k = 0;
        for (auto d1 : note_occurrence) {
            std::cout << labels[k] << " ";
            for (int val : d1) {
                std::cout << val << " ";
            }
            std::cout << "\n";
            k++;
        }
        std::cout << "\n";
        note_occurrence = std::array<std::array<int, 13>, 13>{0};
    }
}

void MidiAnalyzer::analyze_list(std::vector<std::string> midi_list) {
    assert_containing_dir_path();
    for (auto midi_name : midi_list) {
        analyze(midi_name);
    }
}