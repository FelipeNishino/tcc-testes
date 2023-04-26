#include <array>
#include <functional>
#include <iostream>
#include <set>
#include <thread>
#include "libjson/json.hpp"
#include "engine.hpp"
#include "logger.hpp"
#include "json.hpp"

Engine* Engine::pinstance_{nullptr};
std::mutex Engine::mutex_;

Engine::Engine() {
    std::fstream f;
    nlohmann::json emotion_json;
    Json::read_json(&emotion_json, "data/emotion_midi.json");

    std::random_device rd;
    std::mt19937::result_type seed = rd() ^ (
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
                ).count() +
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
                ).count() );
    
    generator = std::mt19937(seed);
    emotion = Emotion::happy;
    bpm = 0;
    for (auto emo : Emotion::EMO_TO_STR) {
        Logger::log(Logger::LOG_INFO, "<Engine> Lendo json p/ emocao %s", emo.c_str());
        emo_feats[emo].transition_count = emotion_json["emotions"][emo]["transition_count"];
        emo_feats[emo].keys = emotion_json["emotions"][emo]["key_probabilities"].get<std::map<int, double>>();
        emo_feats[emo].mode = emotion_json["emotions"][emo]["mode"];
        emo_feats[emo].bpms = emotion_json["emotions"][emo]["tempos"].get<std::vector<int>>();
        std::vector<std::vector<double>> m(7, std::vector<double>(7, 0));
        emo_feats[emo].note_chain = new Markov(m);

        Logger::log(Logger::LOG_INFO, "<Engine> Emoção %s carregada", emo.c_str());
        // emotion_to_durations[emo] = emotion_json["emotions"][emo]["durations_prob_matrix"].get<std::map<double, double>>();
    }

    default_octave = 4;
    count_notas = 0;
}

Engine *Engine::GetInstance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr) {
        pinstance_ = new Engine();
    }
    return pinstance_;
}

int Engine::get_note() {
    int note = states.note_state;
    
    states.note_state = emo_feats[emotion.str()].note_chain->proximo_estado(note);
    count_notas++;
    Logger::log(Logger::LOG_INFO, "Nota sorteada <%d>", (mode[note] + key.load()) + 12 * default_octave);
    return (mode[note] + key.load()) + 12 * default_octave;
}

// double Engine::get_duration() {
//     double val = std::generate_canonical<double,std::numeric_limits<double>::digits>(generator) * 100.0;
//     double sum{};
//     int prox{};
//     for (auto prob : emotion_to_durations[EMO_TO_STR[emotion]]) {
//         sum += prob.second;
//         prox = prob.first;

//         // if (val < sum) break;  
//     }
    
//     // if (prox == 12) {
//         // prox = proximo_estado(atual);
//     // }
//     std::cout << "Soma: " << sum << '\n';
//     std::cout << "Duracao: " << prox << '\n';
//     return prox;    
// }

void Engine::get_bpm() {
    std::random_device rd;
    bpm.store(emo_feats[emotion.str()].bpms.at(rd() % (emo_feats[emotion.str()].bpms.size() - 1)));
}

void Engine::get_mode() {
    mode_type.store(emo_feats[emotion.str()].mode);
    
    if (mode_type.load()) {
        mode = {0, 2, 4, 5, 7, 9 ,11};
    }
    else {
        mode = {0, 2, 3, 5, 7, 8 ,10};
    }
}

void Engine::get_key() {
    double val = std::generate_canonical<double,std::numeric_limits<double>::digits>(generator) * 100.0;
    double sum{};
    int prox{};
    //TODO: Verificar se um mapa em c++ é ordenado, pois a ordem do mapa afeta o processo de selecão do tom
    for (auto &[key, prob] : emo_feats[emotion.str()].keys) {
        sum += prob;
        prox = key;

        if (val < sum) break;  
    }
    key.store(prox);
}

void Engine::get_emotion() {
    std::cout << "Current emotion: " << emotion.str() << '\n';
    for (unsigned int i = 0; i < 4; i++) {
		std::cout << Emotion::EMO_TO_STR[i] << " - " << i << '\n';
	}
    std::cout << "Choose emotion [0-" << 3 << "]: ";
    std::string input;
    int new_emotion = -1;
    
    while ((new_emotion < 0 || new_emotion > 3) && new_emotion != 9) {
        std::cin >> input;
        new_emotion = std::stoi(input);
        std::cout << "Invalid number, try again: ";
    }
    
    if (new_emotion == 9) {
        std::cout << "Exiting...\n";
        wrapper.set_done();
        new_emotion = 0;
    }
    else {
        emotion = (static_cast<Emotion::emotions>(new_emotion));
    }
}

void Engine::listen_to_emotion_input() {
    while (true) {
        get_emotion();
        std::cout << "\x1B[2J\x1B[H";
        get_bpm();
        std::cout << "BPM: " << bpm.load() << '\n';
        get_mode();
        std::cout << "Escala: " << (mode_type.load() ? "Maior" : "Menor") << '\n';
        get_key();
        std::cout << "Tom: " << key.load() << '\n';
        get_note_probabilities();
    }
}

void Engine::get_note_probabilities() {
    int _key = key.load();
    int i{}, j{}, mode_index_i{}, mode_index_j{};
    std::vector<int> total_in_mode(7, 0);
    
    for (mode_index_i = 0; mode_index_i < 7; mode_index_i++) {
        i = (mode[mode_index_i] + _key) % 12;
        for (mode_index_j = 0; mode_index_j < 7;) {
            j = (mode[mode_index_j++] + _key) % 12;
            // Logger::log(Logger::LOG_DEBUG, "<Engine> Vai somar o valor em [%d][%d]", i, j);
            total_in_mode[mode_index_i] += emo_feats[emotion.str()].transition_count[i][j];
        }
    }

    for (mode_index_i = 0; mode_index_i < 7; mode_index_i++) {
        i = (mode[mode_index_i] + _key) % 12;
        for (mode_index_j = 0; mode_index_j < 7; mode_index_j++) {
            j = (mode[mode_index_j] + _key) % 12;            
            emo_feats[emotion.str()].note_chain->matriz_transicao[mode_index_i][mode_index_j] = (double)emo_feats[emotion.str()].transition_count[i][j] * 100.0 / total_in_mode[mode_index_i];
            // Logger::log(Logger::LOG_DEBUG, "<Engine> Prob [%d][%d] = %f", mode_index_i, mode_index_j, emo_feats[emotion.str()].note_chain->matriz_transicao[mode_index_i][mode_index_j]);
        }
    }

    // for (mode_index_i = 0; mode_index_i < 7; mode_index_i++) {
    //     i = (mode[mode_index_i] + _key) % 12;
    //     for (mode_index_j = 0; mode_index_j < 7; mode_index_j++) {
    //         j = (mode[mode_index_j] + _key) % 12;
    //         std::cout << emo_feats[emotion.str()].transition_count[i][j] << ", ";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "\n";
    // for (i = 0; i < 7; i++) {
    //     std::cout << total_in_mode[i] << "   ";    
    // }
    // std::cout << "\n";
    // std::cout << "\n";
    // for (i = 0; i < 7; i++) {
    //     for (j = 0; j < 7; j++) {
    //         std::cout << emo_feats[emotion.str()].note_chain->matriz_transicao[i][j] << ", ";
    //     }   
    //     std::cout << "\n";
    // }
    
    Logger::log(Logger::LOG_INFO, "Escala: <%s>", [this, _key]()->std::string {
        std::string str;
        for (int i = 0; i < 7; i++) str += std::to_string((mode[i] + _key) + 12 * default_octave) + " ";
        return str;
    }().c_str());
    
    Logger::log(Logger::LOG_INFO, "<Engine> Terminou de contabilizar as probabilidades");
}

void Engine::play() {
    get_emotion();
    std::cout << "\x1B[2J\x1B[H";
    get_bpm();
    std::cout << "BPM: " << bpm.load() << '\n';
    get_mode();
    std::cout << "Escala: " << (mode_type.load() ? "Maior" : "Menor") << '\n';
    get_key();
    std::cout << "Tom: " << key.load() << '\n';
    get_note_probabilities();
    

    std::thread first(&Engine::listen_to_emotion_input, this);

    try {
	    wrapper.toggle_stream(true);
	}
	catch ( RtAudioError &error ) {
        std::cout << "Erro" << std::endl;
	    error.printMessage();
        return;
	}

	// Block waiting until callback signals done.
	while ( !wrapper.is_done() )
        stk::Stk::sleep(1000);
	
	// Shut down the callback and output stream.
    Logger::log(Logger::LOG_INFO, "<Engine> Fechando a stream...");
	try {
	    wrapper.toggle_stream(false);
	}
	catch ( RtAudioError &error ) {
            Logger::log(Logger::LOG_ERROR, "<Engine> Erro ao fechar a stream");

	    error.printMessage();
        return;
	}
}