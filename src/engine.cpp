#include <array>
#include <functional>
#include <iostream>
#include <set>
#include <thread>
#include "libjson/json.hpp"
#include "engine.hpp"
#include "utils.hpp"

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
    f.open("data/emotion_midi.json", std::ios::in);
    if (f.fail())
        std::cout << "deu ruim" << std::endl;
    else
        emotion_json = nlohmann::json::parse(f);
    f.close();

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
    emotion.store(none);
    bpm = 0;
    std::cout << "Tô aqui\n";
    for (auto emo : EMO_TO_STR) {
        matriz = emotion_json["emotions"][emo]["prob_matrix"];
        emotion_to_cadeia_notas.insert(std::make_pair(emo, new Markov(matriz)));
        
        vetor = emotion_json["emotions"][emo]["tempos"].get<std::vector<int>>();
        
        // emotion_to_cadeia_notas.emplace("sad", Markov(matriz));
        emotion_to_bpms[emo] = vetor;

        emotion_to_durations[emo] = emotion_json["emotions"][emo]["durations_prob_matrix"].get<std::map<double, double>>();
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
    states.note_state = emotion_to_cadeia_notas[EMO_TO_STR[emotion]]->proximo_estado(note);
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
    bpm.store(emotion_to_bpms[EMO_TO_STR[emotion.load()]].at(rd() % (emotion_to_bpms[EMO_TO_STR[emotion.load()]].size() - 1)));
}

void Engine::get_emotion() {
    std::cout << "Current emotion: " << EMO_TO_STR[emotion.load()] << '\n';
    for (unsigned int i = 0; i < 4; i++) {
		std::cout << EMO_TO_STR[i] << " - " << i << '\n';
	}
    std::cout << "Choose emotion [0-" << 3 << "]: ";
    std::string input;
    int new_emotion = -1;
    
    while (new_emotion < 0 || new_emotion > 3) {
        std::cin >> input;
        new_emotion = std::stoi(input);
        std::cout << "Invalid number, try again: ";
    }
    
    emotion.store(static_cast<Emotion>(new_emotion));
}

void Engine::listen_to_emotion_input() {
    while (true) {
        get_emotion();
        std::cout << "\x1B[2J\x1B[H";
        get_bpm();
        
        std::cout << "BPM: " << bpm.load() << '\n';
    }
}

void Engine::play() {
    if (emotion.load() == none) {
        get_emotion();
        std::cout << "\x1B[2J\x1B[H";
        get_bpm();
        std::cout << "BPM: " << bpm.load() << '\n';
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
	try {
	    wrapper.toggle_stream(false);
	}
	catch ( RtAudioError &error ) {
	    error.printMessage();
        return;
	}
}