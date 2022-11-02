#ifndef TCC_MIDI_ANALYZER_H
#define TCC_MIDI_ANALYZER_H

#include "libjson/json.hpp"
#include "libmidifile/MidiEvent.h"
#include "libmidifile/MidiFile.h"
#include <string>
#include <vector>

struct midi_containing_dir_empty : public std::exception {
   	const char * what () const throw () {
		return "Containing directory for midi files isn't set";
   	}
};

enum NoteOffType {
    chord,
    note
};

struct NoteOff {
    smf::MidiEvent note;
    enum NoteOffType type;
    bool set = false;
};

struct CurrentNote {
    smf::MidiEvent note;
    int key;
};

enum NoteEventType {
    on,
    off   
};

struct LastNoteEvent {
    smf::MidiEvent note;
    NoteEventType type;
};

enum EventType {
    note_on,
    note_off,
    time_signature,
    key_signature,
    tempo,
    other,
};

class MidiAnalyzer {
    private:
        std::string containing_dir;
        void analyze(std::string midi_name);
    public:
        void set_containing_dir(std::string path) { containing_dir = path; }
        void assert_containing_dir_path();
        EventType get_event_type(smf::MidiEvent ev);
        void analyze_list(std::vector<std::string> midi_list);
};

#endif //TCC_MIDI_ANALYZER_H