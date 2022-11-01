#include <cstdlib>
#include <functional>
#include <iostream>
#include "engine.hpp"
#include "device_manager.hpp"
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>
#include "libcurlpp/curlpp/Infos.hpp"
#include "libmidifile/MidiFile.h"
#include "midi_analyzer.hpp"
#include "request_manager.hpp"
#include "emotion_categorizer.hpp"

static const std::vector<std::string> MAIN_OPTIONS = {"-f", "--audio-features", "-d", "--set-device", "-c", "--convert", "-a", "--analyze", "--no-play"};
static unsigned int flags{};

static const std::vector<std::string> MIDIS = {
"Jazz/10-My_Way_-_Frank_Sinatra_-_Alto_Saxophone.mid",
"Jazz/1-FLY_ME_TO_THE_MOON.mid",
"Jazz/2-The_Entertainer_-_Scott_Joplin.mid",
"Jazz/3-Ballade_Pour_Adeline.mid",
"Jazz/4-Ylang_Ylang_-_FKJ__Transcribed_by_LilRoo.mid",
"Jazz/5-Maple_Leaf_Rag_Scott_Joplin.mid",
"Jazz/6-Youre_A_Mean_One_Mr._Grinch_jazz_virtuoso_piano_arr..mid",
"Jazz/7-Linus_and_Lucy.mid",
"Jazz/8-Take_Five_-_Dave_Brubeck-_.mid",
"Jazz/9-MOON_RIVER_.mid",
"Metal/10-Hello__piano.mid",
"Metal/1-In_the_End_-_Linkin_Park.mid",
"Metal/2-Fairy_Tail_Sad_Theme.mid",
"Metal/3-Sonne.mid",
"Metal/4-The_Unforgiven.mid",
"Metal/5-What_Ive_Done_-_Linkin_Park.mid",
"Metal/6-Metallica_One.mid",
"Metal/7-Through_the_Fire_and_Flames.mid",
"Metal/8-Mary_On_A_Cross.mid",
"Metal/9-Metallica_Enter_Sandman.mid",
"Pop/10-Listen_To_Your_Heart__Roxette.mid",
"Pop/1-Someone_You_Loved.mid",
"Pop/2-Never_Gonna_Give_You_Up.mid",
"Pop/3-Lovely_-_Billie_Eilish_with_Khalid.mscz.mid",
"Pop/4-ColdPlay-_Viva_la_vida.mid",
"Pop/5-Take_on_Me_-_a-ha.mid",
"Pop/6-Something_Just_Like_This-The_Chainsmokers.mid",
"Pop/7-Rihanna_ft._Mikky_Echo_-_Stay.mid",
"Pop/8-Marshmello_ft._Bastille_-_Happier.mscz.mid",
"Pop/9-Dynamite__BTS.mid",
"Reggae/10-Meu_Abrigo_C_-_melodia_e_cifras.mid",
"Reggae/1-Petit_Fleur_-_Sidney_Bechet.mid",
"Reggae/2-Dois_Coraes_-_Melim_Partitura.mid",
"Reggae/3-It_Must_Be_Love.mid",
"Reggae/4-Buffalo_Soldier.mid",
"Reggae/5-Rude_-_MAGIC.mid",
"Reggae/6-Santeria.mscz.mid",
"Reggae/7-Madness_-_Our_House.mscz.mid",
"Reggae/8-I_This_Love__Bob_Marley.mid",
"Reggae/9-One_Step_Beyond.mid",
};


enum MAIN_FLAGS {
    FLAG_MAKE_REQUEST = 1,
    FLAG_SET_DEVICE = 1 << 1,
    FLAG_NO_PLAY = 1 << 2,
    FLAG_CONVERT = 1 << 3,
    FLAG_ANALYZE = 1 << 4,
    FLAG_INVALID = 1 << 5
};

void convert() {
    smf::MidiFile midifile;
    int i = 0;
    std::string output;
    for (i = 0; i<40; i++) {
        midifile.read(MIDIS[i]);
        
        if (!midifile.status()) {
            std::cout << "FALHA, ";
        }
        std::cout << "tracks: " << midifile.getTrackCount() << ", song: " << MIDIS[i] << '\n';
        
        output = MIDIS[i];
        output.replace(output.length() - 3, 3, "txt");
        midifile.writeBinascWithComments(output);
        // std::cout << output << '\n';
    }
    
    // midifile.read("/home/nishi/Projects/tcc-testes/data/midi/Reggae/7-Madness_-_Our_House.mscz.mid");
    // std::cout << midifile.status() << std::endl;
    // midifile.joinTracks();
    // midifile.writeBinascWithComments("/home/nishi/Projects/tcc-testes/data/midi/Reggae/7-Madness_-_Our_House.mscz.txt");
}

void help() {
    std::cout << "Usage: ./tcc-test.out [OPTIONS]\n";
    std::cout << "-f\t--audio-features\tMake request for spotify API\n";
    std::cout << "-d\t--set-device\t\tForce prompt for setting default audio device\n";
    std::cout << "-f\t--no-play\t\tDo not execute playback algorithm\n";
    std::cout << "-c\t--convert\t\tConvert Midi file to txt\n";
    std::cout << "-a\t--analyze\t\tAnalize Midi files\n";
    if ((flags & 1 << 2) == 1 << 2) {
        std::cout << "\nInvalid flag usage: received only --no-play, won't do anything\n";
    }
}

void analyzer() {
    MidiAnalyzer ma;
    
    ma.set_containing_dir("/home/nishi/Projects/tcc-testes/data/midi/");
    ma.analyze(MIDIS);
}

void requests() {
    RequestManager rm;
    
    // rm.request_track_feature_by_id({
    //     "3zLTPuucd3e6TxZnu2dlVS",
    //     "5uGZZvIVksQSU7WaVJach5Q",
    //     "2VgU1C40z6KtqDs2r6w1q4",
    // });
    rm.request_track_feature_from_list();
    EmotionCategorizer::categorize();
    // rm.request_track_feature_by_ids();
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
            else if (opt.compare("-c") == 0 || opt.compare("--convert") == 0) {
                flags |= FLAG_CONVERT;
            }
            else if (opt.compare("-a") == 0 || opt.compare("--analyze") == 0) {
                flags |= FLAG_ANALYZE;
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

    if (flags & FLAG_INVALID || !(flags ^ 1 << 2)) {
        help();
        exit(EXIT_FAILURE);
    }

    if (flags & FLAG_MAKE_REQUEST )  {
        std::cout << "Performing requests..." << '\n';
        requests();
    }

    if (flags & FLAG_CONVERT )  {
        convert();
    }

    if (flags & FLAG_ANALYZE )  {
        analyzer();
    }

    if (flags & FLAG_SET_DEVICE ) {
        std::cout << "flag dev" << '\n';
        DeviceManager::set_flag(DeviceManager::FORCE_SET_DEVICE); 
    }
    if (flags & FLAG_NO_PLAY) {
        exit(EXIT_SUCCESS);
    }
    if (!flags) std::cout << "no flags" << '\n';

    Engine* engine = Engine::GetInstance(std::vector<int>(25, 1));
    engine->play();
    
    exit(EXIT_SUCCESS);
}
