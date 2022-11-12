#include <algorithm>
#include <functional>
#include <getopt.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include "libcurlpp/curlpp/Infos.hpp"
#include "libmidifile/MidiFile.h"
#include "device_manager.hpp"
#include "emotion_categorizer.hpp"
#include "engine.hpp"
#include "logger.hpp"
#include "midi_analyzer.hpp"
#include "path_helper.hpp"
#include "request_manager.hpp"

// static const std::vector<std::string> MAIN_OPTIONS = {"-f", "--audio-features", "-d", "--set-device", "-c", "--convert", "-a", "--analyze", "--no-play"};
static int flags{};

static const std::vector<std::string> MIDIS = {
"Jazz/1-FLY_ME_TO_THE_MOON.mid",
"Jazz/2-The_Entertainer_-_Scott_Joplin.mid",
"Jazz/3-Ballade_Pour_Adeline.mid",
"Jazz/4-Ylang_Ylang_-_FKJ__Transcribed_by_LilRoo.mid",
"Jazz/5-Maple_Leaf_Rag_Scott_Joplin.mid",
"Jazz/6-Youre_A_Mean_One_Mr._Grinch_jazz_virtuoso_piano_arr..mid",
"Jazz/7-Linus_and_Lucy.mid",
"Jazz/8-Take_Five_-_Dave_Brubeck-_.mid",
"Jazz/9-MOON_RIVER_.mid",
"Jazz/10-My_Way_-_Frank_Sinatra_-_Alto_Saxophone.mid",
"Metal/1-In_the_End_-_Linkin_Park.mid",
"Metal/2-Fairy_Tail_Sad_Theme.mid",
"Metal/3-Sonne.mid",
"Metal/4-The_Unforgiven.mid",
"Metal/5-What_Ive_Done_-_Linkin_Park.mid",
"Metal/6-Metallica_One.mid",
"Metal/7-Through_the_Fire_and_Flames.mid",
"Metal/8-Mary_On_A_Cross.mid",
"Metal/9-Metallica_Enter_Sandman.mid",
"Metal/10-Hello__piano.mid",
"Pop/1-Someone_You_Loved.mid",
"Pop/2-Never_Gonna_Give_You_Up.mid",
"Pop/3-Lovely_-_Billie_Eilish_with_Khalid.mscz.mid",
"Pop/4-ColdPlay-_Viva_la_vida.mid",
"Pop/5-Take_on_Me_-_a-ha.mid",
"Pop/6-Something_Just_Like_This-The_Chainsmokers.mid",
"Pop/7-Rihanna_ft._Mikky_Echo_-_Stay.mid",
"Pop/8-Marshmello_ft._Bastille_-_Happier.mscz.mid",
"Pop/9-Dynamite__BTS.mid",
"Pop/10-Listen_To_Your_Heart__Roxette.mid",
"Reggae/1-Petit_Fleur_-_Sidney_Bechet.mid",
"Reggae/2-Dois_Coraes_-_Melim_Partitura.mid",
"Reggae/3-It_Must_Be_Love.mid",
"Reggae/4-Buffalo_Soldier.mid",
"Reggae/5-Rude_-_MAGIC.mid",
"Reggae/6-Santeria.mscz.mid",
"Reggae/7-Madness_-_Our_House.mscz.mid",
"Reggae/8-I_This_Love__Bob_Marley.mid",
"Reggae/9-One_Step_Beyond.mid",
"Reggae/10-Meu_Abrigo_C_-_melodia_e_cifras.mid",
};

static const std::vector<std::string> MIDIS_SPOTIFY_IDS = {
"7FXj7Qg3YorUxdrzvrcY25",
"0abhXJIOH1NqbsXLaZD2DI",
"3kK2DCW85mxXaFeL0wCvRc",
"2CuUB5MbfRShtunR8g5hjR",
"4AE032Y0x1WPOi5CsmggnU",
"6TXAjof8V0Gr4zFoY00zHR",
"4h7jnrmWCKE5xSDIULYOTE",
"5UbLKRX1qVROlwDFOooEvG",
"2HUAjeHWA7FQNbnWhlboOL",
"3spdoTYpuCpmq19tuD0bOe",
"60a0Rd6pjrkxjPbaKzXjfq",
"7JoZiEMrwjpD18krCV8Czo",
"3gVhsZtseYtY1fMuyYq06F",
"4aYLAF6ckQ5ooGGGM7sWAa",
"18lR4BzEs7e3qzc0KVkTpU",
"5IX4TbIR5mMHGE4wiWwKW0",
"2eB7JqIY4hTTSz31h6bjwR",
"2HZLXBOnaSRhXStMLrq9fD",
"5sICkBXVmaCQk5aISGR3x1",
"0aYUqsvZG7bAslrUkd9Z0g",
"7qEHsqek33rTcFNT9PFqLf",
"4cOdK2wGLETKBW3PvgPWqT",
"0u2P5u6lvoDfwTYjAADbn4",
"1mea3bSkSGXuIRvnydlB5b",
"2WfaOiMkCvy7F5fcp2zZ8L",
"6RUKPb4LETWmmr3iAEQktW",
"0GNI8K3VATWBABQFAzBAYe",
"2dpaYNEQHiRxtZbfNsse99",
"5QDLhrAOJJdNAmCTJ8xMyW",
"5JvsxPAHsGxwNq4xto2HtM",
"3kkLR262kQxpMmPhlN7Gy3",
"4xwPOyN9VgaTPehdsLiYul",
"3q2gRjU2UdOYRDY1p4cNHr",
"7BfW1eoDh27W69nxsmRicb",
"6RtPijgfPKROxEzTHNRiDp",
"2hnMS47jN0etwvFPzYk11f",
"1EXrFPfVNVsyb32yapebbM",
"6JRLFiX9NJSoRRKxowlBYr",
"0bAYJxEe6ate8gL0G2qj1B",
"5U28PY9MekLyCBYtLHGQpe",
};

enum MAIN_FLAGS {
    FLAG_REQUEST = 1,
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
        {"features",    optional_argument,  &flags, FLAG_REQUEST},
        {"help",        no_argument,        &flags, FLAG_HELP},
        {"loglevel",    required_argument,  0, 'l'},
        {"no-play",     no_argument,        &flags, FLAG_NO_PLAY},
        
        {0,             0,                  0,  0 }
    };
    
    int option_index = 0;

    while ((c = getopt_long(argc, argv, "a::cdf::hl:n", options, &option_index)) != -1) {
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
                MidiAnalyzer::set_containing_dir(optarg);
            flags |= FLAG_ANALYZE;
            break;
        case 'c':
            flags |= FLAG_CONVERT;
            break;
        case 'd':
            flags |= FLAG_DEVICE;
            break;
        case 'f':
            if (optarg)
                RequestManager::set_songlist_dir(optarg);
            flags |= FLAG_REQUEST;
            break;
        case 'h':
            flags |= FLAG_HELP;
            break;
        case 'l':
            std::cout << "Recebeu como arg pro l: " << optarg << '\n';
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
        midifile.read("/home/nishi/Projects/tcc-testes/data/midi/" + names[i]);
        
        if (!midifile.status()) {
            std::cout << "FALHA, ";
        }
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
    // ma.set_containing_dir("/home/nishi/Projects/tcc-testes/data/midi/");
    ma.analyze_list(MIDIS, MIDIS_SPOTIFY_IDS);
    // ma.analyze_list({"Metal/6-Metallica_One.mid"});
    // ma.analyze_list({"teste_nnnc.mid", "teste_cnnn.mid"});
}

void requests() {
    RequestManager rm;
    rm.request_track_feature_from_list();
    EmotionCategorizer::categorize();
    // rm.request_track_feature_by_ids();
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

    if (flags & FLAG_CONVERT )  {
        convert();
    }

    if (flags & FLAG_REQUEST )  {
        std::cout << "Performing requests..." << '\n';
        requests();
    }

    if (flags & FLAG_ANALYZE )  {
        analyzer();
    }

    if (flags & FLAG_DEVICE ) {
        DeviceManager::set_flag(DeviceManager::FORCE_SET_DEVICE); 
    }
    if (flags & FLAG_NO_PLAY) {
        exit(EXIT_SUCCESS);
    }

    Engine* engine = Engine::GetInstance(std::vector<int>(25, 1));
    engine->play();
    
    exit(EXIT_SUCCESS);
}
