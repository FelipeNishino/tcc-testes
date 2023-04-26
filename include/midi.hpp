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
    
    // namespace FS {
    //     static bool verify_midi_filename(std::string filename) {
    //     // int underscore_count = 0;
    //         Logger::log(Logger::LOG_DEBUG, "Checking entry: %s", filename.c_str());
    //         Logger::log(Logger::LOG_DEBUG, "Checking size: %d", filename.size());
    //         if (filename.size() < 23) return false;
    //         Logger::log(Logger::LOG_DEBUG, "Checking underscore");
    //         if (filename.at(22) != '_') return false;
    //         Logger::log(Logger::LOG_DEBUG, "Checking id");
    //         for (int i = 0; i < 22; i++) {
    //             if (filename[i] == '_') return false;
    //         }
    //         Logger::log(Logger::LOG_DEBUG, "Ok");
    //         return true;
    //     }

    //     static std::vector<MIDIFileEntry> get_files_from_data_dir() {
    //         namespace fs = std::filesystem;
    //         std::vector<fs::directory_entry> genres;
    //         std::vector<MIDIFileEntry> midi_files;

    //         for(const fs::directory_entry& entry: fs::directory_iterator{"./data/midi"}) {
    //             if (entry.is_directory()) {
    //                 genres.push_back(entry);
    //             }
    //         }

    //         for (auto &genre : genres) {
    //             for(const fs::directory_entry& entry: fs::directory_iterator{genre.path()}) {
    //                 if (entry.path().extension() != ".mid") {
    //                     continue;
    //                 }
    //                 std::string filename = entry.path().filename().string();
    //                 if (!verify_midi_filename(filename)) {
    //                     Logger::log(Logger::LOG_ERROR, "Invalid Midi filename %s. Must have spotify id with trailing underscore at front", filename.c_str());
    //                 }
    //                 else {
    //                     MIDIFileEntry midi_file;
    //                     midi_file.filename = filename;
    //                     midi_file.path = entry.path();
    //                     midi_file.spotify_id = midi_file.filename.substr(0, 22);
    //                     midi_files.push_back(midi_file);
    //                     Logger::log(Logger::LOG_INFO, "Loaded %s.", filename.c_str());
    //                 }
    //             }
    //         }
    //         return midi_files;
    //     }
    // }
    // static int NOTE_TO_FREQ[] = {264, 297, 330, 352, 396, 440, 495, 528};
    // static const int NOTE_TO_MIDI_KEY[] = {60, 62, 64, 65, 67, 69, 71, 72};
    // static const int NOTE_TO_MIDI_KEY[] = {48, 50, 52, 53, 55, 57, 59, 60};
    // static const int NOTE_TO_MIDI_KEY[] = {48, 49, 50, 51, 52, 53, 55, 56, 57, 58, 59, 60};
}

#endif //TCC_MIDI_H