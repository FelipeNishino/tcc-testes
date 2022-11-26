#ifndef TCC_JSON_H
#define TCC_JSON_H

#include <exception>
#include <string>
#include "libjson/json.hpp"
#include <fstream>

namespace Json {
    struct unsuccesful_json_read : public std::exception {
        std::string err_msg;
        public:
        unsuccesful_json_read(std::string path)
        : err_msg(std::string("Couldn't load json with path ") + path)
        {}
        const char * what () const throw () {
            return err_msg.c_str();
        }
    };

    
    
    static void read_json(nlohmann::json* json, std::string path) {
        std::fstream f;

        f.open(path, std::ios::in);
        if (f.fail())
            throw unsuccesful_json_read(path);
        else
            *json = nlohmann::json::parse(f);
        f.close();
    }

    // static int NOTE_TO_FREQ[] = {264, 297, 330, 352, 396, 440, 495, 528};
    // static const int NOTE_TO_MIDI_KEY[] = {60, 62, 64, 65, 67, 69, 71, 72};
    // static const int NOTE_TO_MIDI_KEY[] = {48, 50, 52, 53, 55, 57, 59, 60};
    // static const int NOTE_TO_MIDI_KEY[] = {48, 49, 50, 51, 52, 53, 55, 56, 57, 58, 59, 60};
}

#endif //TCC_JSON_H