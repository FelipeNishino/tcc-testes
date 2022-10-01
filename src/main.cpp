#include <cstdlib>
#include <functional>
#include <iostream>
#include "include/engine.hpp"
#include "include/device_manager.hpp"
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>
#include "include/libmidifile/MidiFile.h"
#include "include/request_manager.hpp"

static const std::vector<std::string> MAIN_OPTIONS = {"-f", "--audio-features", "-d", "--set-device", "--no-play"};
static unsigned int flags{};

enum MAIN_FLAGS {
    FLAG_MAKE_REQUEST = 1,
    FLAG_SET_DEVICE = 1 << 1,
    FLAG_NO_PLAY = 1 << 2,
    FLAG_INVALID = 1 << 3
};

void convert() {
    smf::MidiFile midifile;
    midifile.read("/home/nishi/Downloads/midi/bach_846.mid");
       // midifile.write("output.mid");
    std::cout << midifile.status() << std::endl;
    midifile.joinTracks();
    midifile.writeBinascWithComments("/home/nishi/Downloads/midi/bach_846_joined.mid");
}

void help() {
    std::cout << "Usage: ./tcc-test.out [OPTIONS]\n";
    std::cout << "-f\t--audio-features\tMake request for spotify API\n";
    std::cout << "-d\t--set-device\t\tForce prompt for setting default audio device\n";
    std::cout << "-f\t--no-play\t\tDo not execute playback algorithm\n";
    if ((flags & 1 << 2) == 1 << 2) {
        std::cout << "\nInvalid flag usage: received only --no-play, won't do anything\n";
    }
}

int main(int argc, char* argv[]) {
    using std::string;
    string opt{};
    std::vector<string> argv_str(argc - 1);
    std::transform(&argv[1], &argv[argc], argv_str.begin(), [](char* cs){
        return string{cs};
    });

    for (auto const &opt : argv_str) {
        bool valid_opt = [opt]()->bool {
            auto str = std::find(MAIN_OPTIONS.begin(), MAIN_OPTIONS.end(), opt);
            return str!=MAIN_OPTIONS.end();
        }();
        if (valid_opt) {
            if (opt.compare("-f") == 0 || opt.compare("--audio-features") == 0) {
            flags |= FLAG_MAKE_REQUEST;
            }
            else if (opt.compare("-d") == 0 || opt.compare("--set-device") == 0) {
                flags |= FLAG_SET_DEVICE;
            }
            else {// if (opt.compare("--no-play") == 0) {
                flags |= FLAG_NO_PLAY;
            }
        }
        else {
            std::cerr << "Unrecognized flag " << opt << '\n';
            flags |= FLAG_INVALID;
            break;
        }
    }

    if (flags & FLAG_INVALID || (flags & 1 << 2) == 1 << 2) {
        help();
        exit(EXIT_FAILURE);
    }

    if (flags & FLAG_MAKE_REQUEST ) std::cout << "flag req" << '\n';
    if (flags & FLAG_SET_DEVICE ) std::cout << "flag dev" << '\n';
    if (flags & FLAG_NO_PLAY ) std::cout << "flag nop" << '\n';
    if (!flags) std::cout << "no flags" << '\n';

    // RequestManager rm;
    // rm.perform_request();
    // rm.request_track_feature_by_id("3zLTPuucd3e6TxZnu2dlVS");
    // rm.request_track_feature_by_ids({
        // "3zLTPuucd3e6TxZnu2dlVS",
        // "5uGZZvIVksQSU7WaVJch5Q"
    // });
    // rm.request_track_feature_by_id("5uGZZvIVksQSU7WaVJch5Q");
    
    // std::vector<BYTE> decodedData = base64_decode(encodedData);

    // Engine* engine = Engine::GetInstance(std::vector<int>(25, 1));
    // engine->play();

}
