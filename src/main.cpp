#include <algorithm>
#include <functional>
#include <getopt.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include "libcurlpp/curlpp/Infos.hpp"
#include "libmidifile/MidiFile.hpp"
#include "device_manager.hpp"
#include "emotion_categorizer.hpp"
#include "engine.hpp"
#include "logger.hpp"
#include "json.hpp"
#include "midi_analyzer.hpp"
#include "path_helper.hpp"
#include "request_manager.hpp"
#include <filesystem>

static int flags{};

enum MAIN_FLAGS {
    FLAG_FEATURES = 1,
    FLAG_DEVICE = 1 << 1,
    FLAG_NO_PLAY = 1 << 2,
    FLAG_ANALYZE = 1 << 3,
    FLAG_INVALID = 1 << 4,
    FLAG_HELP = 1 << 5,
};

void usage() {
    std::cout << "Usage: ./tcc-test.out [OPTIONS]\n";
    std::cout << "-f\t--audio-features\tMake request for spotify API\n";
    std::cout << "-d\t--set-device\t\tForce prompt for setting default audio device\n";
    std::cout << "-n\t--no-play\t\tDo not execute playback algorithm\n";
    std::cout << "-c\t--convert\t\tConvert Midi file to txt\n";
    std::cout << "-a\t--analyze\t\tAnalize Midi files\n";
    if (flags == FLAG_NO_PLAY) {
        std::cout << "\nInvalid flag usage: received only --no-play, won't do anything\n";
    }
}

void get_options(int argc, char* const* argv) {
    int c = 0;

    static const struct option options[] = {
        {"analyze",     optional_argument,  &flags, FLAG_ANALYZE},
        {"set-device",  no_argument,        &flags, FLAG_DEVICE},
        {"db",          optional_argument,  &flags, FLAG_FEATURES},
        {"help",        no_argument,        &flags, FLAG_HELP},
        {"loglevel",    required_argument,  0, 'l'},
        {"no-play",     no_argument,        &flags, FLAG_NO_PLAY},
        
        {0,             0,                  0,  0 }
    };
    
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "a::dfhl:n", options, &option_index)) != -1) {
        switch (c) {
        case 0:
          /* If this option set a flag, do nothing else now. */
            if (options[option_index].flag != 0) break;
            printf("option %s", options[option_index].name);
            if (optarg) printf(" with arg %s", optarg);
            printf("\n");
            break;
        case 'a':
            if (optarg)
                DatabaseManager::set_data_dir(optarg);
            flags |= FLAG_ANALYZE;
            break;
        case 'd':
            flags |= FLAG_DEVICE;
            break;
        case 'f':
            flags |= FLAG_FEATURES;
            break;
        case 'h':
            flags |= FLAG_HELP;
            break;
        case 'l':
            Logger::set_output_level(static_cast<Logger::LogLevel>(atoi(optarg)));
            break;
        case 'n':
            flags |= FLAG_NO_PLAY;
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
        }
    }
}

void analyzer() {
    MidiAnalyzer ma;
    ma.analyze_list();
}

void load_database() {
    DatabaseManager* dbm = DatabaseManager::GetInstance();
    RequestManager rm{};
    if (rm.request_track_features()) {
        dbm->reload();
        EmotionCategorizer::categorize();
    }
}

int main(int argc, char* argv[]) {
    using std::string;
    
    PathHelper::set_run_path(argv[0]); 

    string opt{};
    std::vector<string> argv_str(argc - 1);
    get_options(argc, argv);
    
    if (flags & FLAG_INVALID || !(flags ^ 1 << 2)) {
        usage();
        exit(EXIT_FAILURE);
    }

    if (flags & FLAG_FEATURES)  {
        load_database();
    }
    
    if (flags & FLAG_ANALYZE)  {
        analyzer();
    }
    if (flags & FLAG_DEVICE) {
        DeviceManager::set_flag(DeviceManager::FORCE_SET_DEVICE); 
    }
    if (flags & FLAG_NO_PLAY) {
        exit(EXIT_SUCCESS);
    }

    Engine* engine = Engine::GetInstance();
    
    engine->play();
}
