#ifndef TCC_MIDI_ANALYZER_H
#define TCC_MIDI_ANALYZER_H

#include "libjson/json.hpp"
#include "libmidifile/MidiFile.h"
#include <string>
#include <vector>

struct midi_containing_dir_empty : public std::exception {
   	const char * what () const throw () {
		return "Containing directory for midi files isn't set";
   	}
};

class MidiAnalyzer {
    private:
        static std::string containing_dir;
    public:
        static void assert_path_is_set() {
            if (containing_dir.empty()) throw midi_containing_dir_empty(); 
        }

        static void set_containing_dir(std::string path) {
            containing_dir = path;
        }

        nlohmann::json analyze(std::vector<std::string> midi_list);
};

#endif //TCC_MIDI_ANALYZER_H