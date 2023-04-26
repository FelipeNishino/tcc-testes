#include <array>
#include <functional>
#include <iostream>
#include <set>
#include <thread>
#include "libjson/json.hpp"
#include "engine.hpp"
#include "logger.hpp"
#include "json.hpp"

/**
 * Static methods should be defined outside the class.
 */

Engine* Engine::pinstance_{nullptr};
std::mutex Engine::mutex_;

/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value. RU:
 */

Engine::Engine() {
    std::fstream f;
    nlohmann::json emotion_json;
    Json::read_json(&emotion_json, "data/emotion_midi.json");

    std::vector<std::vector<double>> matriz;
    std::vector<int> vetor{};
    std::random_device rd;
    // seed value is designed specifically to make initialization
    // parameters of std::mt19937 (instance of std::mersenne_twister_engine<>)
    // different across executions of application
    // const auto p0 = 
    // std::mt19937::result_type seed = rd() ^ (
    //         (std::mt19937::result_type)
    //         std::chrono::duration_cast<std::chrono::seconds>(
    //             std::chrono::system_clock::now().time_since_epoch()
    //             ).count() +
    //         (std::mt19937::result_type)
    //         std::chrono::duration_cast<std::chrono::microseconds>(
    //             std::chrono::high_resolution_clock::now().time_since_epoch()
    //             ).count() );

    std::mt19937::result_type seed = rd() ^ (
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
                ).count() +
            (std::mt19937::result_type)
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
                ).count() );

    // std::mt19937 gen(seed);

    // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    
    std::cout << "Tô aqui\n";
    generator = std::mt19937(std::chrono::time_point<std::chrono::high_resolution_clock>{}.time_since_epoch().count());
    emotion = Emotion::happy;
    bpm = 0;
    std::cout << "Tô aqui\n";
    for (auto emo : Emotion::EMO_TO_STR) {
        Logger::log(Logger::LOG_INFO, "<Engine> Lendo json p/ emocao %s", emo.c_str());
        emo_feats[emo].transition_count = emotion_json["emotions"][emo]["total_note_matrix"];
        emo_feats[emo].keys = emotion_json["emotions"][emo]["key_prob_array"].get<std::map<int, double>>();
        emo_feats[emo].mode = emotion_json["emotions"][emo]["mode"];
        emo_feats[emo].bpms = emotion_json["emotions"][emo]["tempos"].get<std::vector<int>>();
        std::vector<std::vector<double>> m(7, std::vector<double>(7, 0));
        emo_feats[emo].note_chain = new Markov(m);
    }
    
    // emotion_to_cadeia_notas.emplace("angry", Markov(emotion_json["emotions"]["angry"]["note_matrix"]));
    // emotion_to_cadeia_notas.emplace("relaxed", Markov(emotion_json["emotions"]["relaxed"]["note_matrix"]));

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
    return note + 12 * default_octave;
}

double Engine::get_duration() {
    double val = std::generate_canonical<double,std::numeric_limits<double>::digits>(generator) * 100.0;
    double sum{};
    int prox{};
    for (auto prob : emotion_to_durations[EMO_TO_STR[emotion]]) {
        sum += prob.second;
        prox = prob.first;

        // if (val < sum) break;  
    }
    
    // if (prox == 12) {
        // prox = proximo_estado(atual);
    // }
    std::cout << "Soma: " << sum << '\n';
    std::cout << "Duracao: " << prox << '\n';
    return prox;    
}

void Engine::get_bpm() {
    std::random_device rd;
    bpm.store(emo_feats[emotion.str()].bpms.at(rd() % (emo_feats[emotion.str()].bpms.size() - 1)));
}

void Engine::get_mode() {
    mode.store(emotion_to_mode[EMO_TO_STR[emotion.load()]]);
}

void Engine::get_key() {
    double val = std::generate_canonical<double,std::numeric_limits<double>::digits>(generator) * 100.0;
    double sum{};
    int prox{};
    for (auto &[key, prob] : emo_feats[emotion.str()].keys) {
        sum += prob;
        prox = key;

        // if (val < sum) break;  
    }
    
    // if (prox == 12) {
        // prox = proximo_estado(atual);
    // }
    // std::cout << "Soma: " << sum << '\n';
    // std::cout << "Duracao: " << prox << '\n';
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
        std::cout << "Escala: " << (mode.load() ? "Maior" : "Menor") << '\n';
        get_key();
        std::cout << "Tom: " << key.load() << '\n';
    }
}

void Engine::play() {
        get_emotion();
        std::cout << "\x1B[2J\x1B[H";
        get_bpm();
        std::cout << "BPM: " << bpm.load() << '\n';
        Logger::log(Logger::LOG_ERROR, "Tô aquiasdac");
        get_mode();
        std::cout << "Escala: " << (mode.load() ? "Maior" : "Menor") << '\n';
        get_key();
        std::cout << "Tom: " << key.load() << '\n';
    }

    std::thread first(&Engine::listen_to_emotion_input, this);

    // int TOTAL = 20825;
    // std::array<std::array<int, 13>, 13> hist{0};
    
    // for (int i = 0; i < TOTAL; i++) {
    //     int past = get_note();
    //     hist[past][states.note_state]++;
    // }
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
    Logger::log(Logger::LOG_INFO, "Fechando a stream...");
	try {
	    wrapper.toggle_stream(false);
	}
	catch ( RtAudioError &error ) {
            Logger::log(Logger::LOG_INFO, "N conseguiu fechar...");

	    error.printMessage();
        return;
	}
}