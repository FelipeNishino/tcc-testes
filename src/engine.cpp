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
    emotion.store(none);
    bpm = 0;
    
    for (auto emo : EMO_TO_STR) {
        matriz = emotion_json["emotions"][emo]["prob_matrix"];
        emotion_to_cadeia_notas.insert(std::make_pair(emo, new Markov(matriz)));
        
        vetor = emotion_json["emotions"][emo]["tempos"].get<std::vector<int>>();
        
        // emotion_to_cadeia_notas.emplace("sad", Markov(matriz));
        emotion_to_bpms[emo] = vetor;
    }
    
    // emotion_to_cadeia_notas.emplace("angry", Markov(emotion_json["emotions"]["angry"]["note_matrix"]));
    // emotion_to_cadeia_notas.emplace("relaxed", Markov(emotion_json["emotions"]["relaxed"]["note_matrix"]));

    default_octave = 5;
    count_notas = 0;
}

Engine *Engine::GetInstance(std::vector<int> m) {
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
    return note;
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