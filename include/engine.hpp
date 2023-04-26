#ifndef TCC_ENGINE_H
#define TCC_ENGINE_H

#include <atomic>
#include <map>
#include <mutex>
#include "markov.hpp"
#include "stk_wrapper.hpp"

// class Engine {
//     public:
//         Markov cadeia_notas;
//         StkWrapper wrapper;
//         Engine(int n, int* m_notas)
//         :
//         cadeia_notas(n, m_notas)
//         {}
//         void get_nota();
//         void play();
// };

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

        std::map<std::string, EmotionFeatures> emo_feats;
    protected:
        Engine();
        ~Engine() {}
    public:        
        States states;
        std::mt19937 generator;
        Emotion emotion;
        std::atomic<int> bpm;
        std::atomic<int> mode;
        std::atomic<int> key;
        int default_octave;
        StkWrapper wrapper;
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
        static Engine *GetInstance();

        // std::string value() const{
        //     return value_;
        // } 
};



#endif //TCC_ENGINE_H