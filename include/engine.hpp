#ifndef TCC_ENGINE_H
#define TCC_ENGINE_H

#include <atomic>
#include <map>
#include <mutex>
#include "markov.hpp"
#include "stk_wrapper.hpp"
#include "emotion.hpp"

struct States {
    int note_state{};
    int tempo_state{};
    // double duration_state{};
};

struct EmotionFeatures {    
    Markov* note_chain;
    std::vector<std::vector<int>> transition_count;
    std::vector<int> bpms;
    // std::map<double, double> durations;
    std::map<int, double> keys;
    int mode;
};

class Engine {
    private:
        static Engine * pinstance_;
        static std::mutex mutex_;
        std::vector<int> mode;
        std::map<std::string, EmotionFeatures> emo_feats;
    protected:
        Engine();
        ~Engine() {}
    public:        
        States states;
        std::mt19937 generator;
        Emotion emotion;
        std::atomic<int> bpm;
        std::atomic<int> mode_type;
        std::atomic<int> key;
        int default_octave;
        int get_note();
        int count_notas;
        void play();
        void get_emotion();
        
        void listen_to_emotion_input();
        // Singletons should not be cloneable
        Engine(Engine &other) = delete;
        // Singletons should not be assignable.
        void operator=(const Engine &) = delete;
        void get_bpm();
        void get_mode();
        void get_key();
        // double get_duration();
        void get_note_probabilities();
        static Engine *GetInstance();
        StkWrapper wrapper;
};

#endif //TCC_ENGINE_H