#ifndef TCC_MIDI_ANALYZER_H
#define TCC_MIDI_ANALYZER_H

#include <array>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "libmidifile/MidiEvent.hpp"
#include "libmidifile/MidiFile.hpp"
#include "database_manager.hpp"
// #include <filesystem>
// #include "midi.hpp"

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
    smf::MidiEvent* ev;
    int key;
    ComponentType component;
    bool set = false;
};

struct SongEventCollections {
    std::vector<smf::MidiEvent*> note_on_evs;
    std::vector<smf::MidiEvent*> note_off_evs;
    std::vector<smf::MidiEvent*> time_sig_evs;
    std::vector<smf::MidiEvent*> key_sig_evs;
    std::vector<smf::MidiEvent*> tempo_evs;
    std::vector<smf::MidiEvent*> other_evs;
};

struct Quarter {
    std::vector<std::vector<int>> transitions;
    std::vector<int> label;
};

struct TrackInfo {
    std::array<std::array<int, 13>, 13> note_matrix{0};
    std::map<int, std::list<int>> bpm_timestamp;
    std::map<double, int> note_durations;
    Quarter quarters;
    std::map<int, int> octaves;
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
        MidiFeatures analyze(std::string midi_name);
        void display_note_ocurrence(TrackInfo track);
    public:
        EventType get_event_type(smf::MidiEvent ev);
        void analyze_list(std::vector<MidiFSEntry> midi_list = {});
};

#endif //TCC_MIDI_ANALYZER_H