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
#include "midi.hpp"
#include "json.hpp"
#include "midi_analyzer.hpp"
#include "path_helper.hpp"
#include "request_manager.hpp"
#include <filesystem>
// Ordem das musicas no yt
// 9 4 2 0 1 3 8 6 5 7 
// static const std::vector<std::string> MAIN_OPTIONS = {"-f", "--audio-features", "-d", "--set-device", "-c", "--convert", "-a", "--analyze", "--no-play"};
static int flags{};

enum MAIN_FLAGS {
    FLAG_FEATURES = 1,
    FLAG_DEVICE = 1 << 1,
    FLAG_NO_PLAY = 1 << 2,
    FLAG_CONVERT = 1 << 3,
    FLAG_ANALYZE = 1 << 4,
    FLAG_INVALID = 1 << 5,
    FLAG_HELP = 1 << 6,
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
    int ret = 0;

    static const struct option options[] = {
        {"analyze",     optional_argument,  &flags, FLAG_ANALYZE},
        {"convert",     no_argument,        &flags, FLAG_CONVERT},
        {"set-device",  no_argument,        &flags, FLAG_DEVICE},
        {"db",          optional_argument,  &flags, FLAG_FEATURES},
        {"help",        no_argument,        &flags, FLAG_HELP},
        {"loglevel",    required_argument,  0, 'l'},
        {"no-play",     no_argument,        &flags, FLAG_NO_PLAY},
        
        {0,             0,                  0,  0 }
    };
    
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "a::cdfhl:n", options, &option_index)) != -1) {
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
        case 'c':
            flags |= FLAG_CONVERT;
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

void convert() {
    smf::MidiFile midifile;
    int i = 0;
    std::string output;
    // std::vector<std::string> names = {"teste_nnnc.mid", "teste_cnnn.mid"};
    std::vector<std::string> names = {"Metal/6-Metallica_One.mid"};
    // std::vector<std::string> names = MIDIS;
    for (i = 0; i < names.size(); i++) {
        Midi::read_midi(&midifile, "data/midi/" + names[i]);
        std::cout << "tracks: " << midifile.getTrackCount() << ", song: " << names[i] << '\n';
        
        output = names[i];
        output.replace(output.length() - 3, 3, "txt");
        midifile.joinTracks();
        midifile.writeBinascWithComments(output);
        // std::cout << output << '\n';
    }
    
    // midifile.read("/home/nishi/Projects/tcc-testes/data/midi/Reggae/7-Madness_-_Our_House.mscz.mid");
    // std::cout << midifile.status() << std::endl;
    // midifile.joinTracks();
    // midifile.writeBinascWithComments("/home/nishi/Projects/tcc-testes/data/midi/Reggae/7-Madness_-_Our_House.mscz.txt");
}

void analyzer() {
    // MidiAnalyzer::set_containing_dir("data/midi/");
    MidiAnalyzer ma;
    ma.analyze_list();
    // ma.analyze_list({"Metal/6-Metallica_One.mid"});
    // ma.analyze_list({"teste_nnnc.mid", "teste_cnnn.mid"});
}

void load_database() {
    DatabaseManager* dbm = DatabaseManager::GetInstance();
    RequestManager rm{};
    if (rm.request_track_features()) {
        dbm->reload();
        EmotionCategorizer::categorize();
    }
}

void teste() {
    nlohmann::json emotion_json;
    std::fstream f;
    std::map<std::string, std::map<double, double>> emotion_to_durations;
    Json::read_json(&emotion_json, "data/emotion_midi.json");
    std::map<double, double> relaxed_duration;
    std::map<double, double> sad_duration;
    std::map<double, double> angry_duration;
    std::map<double, double> happy_duration;

    // emotion_to_durations[emo] = emotion_json["emotions"][emo]["durations_prob_matrix"].get<std::map<double, double>>();
    relaxed_duration = emotion_json["emotions"]["relaxed"]["durations_prob_matrix"].get<std::map<double, double>>();
    sad_duration = emotion_json["emotions"]["sad"]["durations_prob_matrix"].get<std::map<double, double>>();
    angry_duration = emotion_json["emotions"]["angry"]["durations_prob_matrix"].get<std::map<double, double>>();
    happy_duration = emotion_json["emotions"]["happy"]["durations_prob_matrix"].get<std::map<double, double>>();
    double highest_prob_duration = 0;
    double highest_duration = 0;
    for (auto &[dur, prob] : relaxed_duration) {
        if (prob > highest_prob_duration) {
            highest_prob_duration = prob;
            highest_duration = dur;
        }
    }
    std::cout << "Maior duração para relaxed: ";
    std::cout.precision(17);
    std::cout << highest_duration << " , com prob de: " << highest_prob_duration << "\n";
    highest_prob_duration = 0;
    highest_duration = 0;

    for (auto &[dur, prob] : sad_duration) {
        if (prob > highest_prob_duration) {
            highest_prob_duration = prob;
            highest_duration = dur;
        }
    }
    std::cout << "Maior duração para sad: ";
    std::cout.precision(17);
    std::cout << highest_duration << " , com prob de: " << highest_prob_duration << "\n";
    highest_prob_duration = 0;
    highest_duration = 0;
    
    for (auto &[dur, prob] : happy_duration) {
        if (prob > highest_prob_duration) {
            highest_prob_duration = prob;
            highest_duration = dur;
        }
    }

    std::cout << "Maior duração para happy: ";
    std::cout.precision(17);
    std::cout << highest_duration << " , com prob de: " << highest_prob_duration << "\n";
    highest_prob_duration = 0;
    highest_duration = 0;
    for (auto &[dur, prob] : angry_duration) {
        if (prob > highest_prob_duration) {
            highest_prob_duration = prob;
            highest_duration = dur;
        }
    }
    std::cout << "Maior duração para angry: ";
    std::cout.precision(17);
    std::cout << highest_duration << " , com prob de: " << highest_prob_duration << "\n";

    // for (auto emo : EMO_TO_STR) {
        // matriz = emotion_json["emotions"][emo]["prob_matrix"];
        // emotion_to_cadeia_notas.insert(std::make_pair(emo, new Markov(matriz)));
        
        // vetor = emotion_json["emotions"][emo]["tempos"].get<std::vector<int>>();
        
        // emotion_to_cadeia_notas.emplace("sad", Markov(matriz));
        // emotion_to_bpms[emo] = vetor;
        
        // emotion_to_durations[emo] = emotion_json["emotions"][emo]["durations_prob_matrix"].get<std::map<double, double>>();
        // emotion_to_durations[emo]

    // }
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
    
    if (flags & FLAG_CONVERT)  {
        convert();
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

    // Engine* engine = Engine::GetInstance(std::vector<int>(25, 1));
    
    engine->play();
    // exit(EXIT_SUCCESS);
}
