#ifndef TCC_MIDI_ANALYZER_H
#define TCC_MIDI_ANALYZER_H

#include <array>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "libmidifile/MidiEvent.h"
#include "libmidifile/MidiFile.h"

struct midi_containing_dir_empty : public std::exception {
   	const char * what () const throw () {
		return "Containing directory for midi files isn't set";
   	}
};

enum EventType {
    note_on,
    note_off,
    time_signature,
    key_signature,
    tempo,
    other,
};

enum ComponentType {
    chord,
    note
};

enum NoteEventType {
    on,
    off
};

struct NoteEvent {
    smf::MidiEvent ev;
    int key;
    ComponentType component;
    bool set = false;
};

struct TrackInfo {
    std::array<std::array<int, 13>, 13> note_matrix{0};
    std::map<int, std::list<int>> bpm_timestamp;
    int note_count{};
};

struct MidiFeatures {
    TrackInfo track_info;
    std::set<int> bpms;
};

struct TrackState {
    NoteEventType prev_note_type;
    NoteEvent prev_on;
    NoteEvent prev_off;
    NoteEvent curr_ev;
};

class MidiAnalyzer {
    private:
        static inline std::string containing_dir = "data/midi/";
        void assert_containing_dir_path();
        MidiFeatures analyze(std::string midi_name);
        void display_note_ocurrence(TrackInfo track);
    public:
        static void set_containing_dir(std::string path) { containing_dir = path; }
        EventType get_event_type(smf::MidiEvent ev);
        void analyze_list(std::vector<std::string> midi_list, std::vector<std::string> spotify_ids);
};

#endif //TCC_MIDI_ANALYZER_H