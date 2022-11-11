#ifndef TCC_ENGINE_H
#define TCC_ENGINE_H

#include "markov.hpp"
#include "stk_wrapper.hpp"
#include "utils.hpp"
#include <array>
#include <mutex>
#include <map>
#include <atomic>

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

enum Emotion {
    happy,
    sad,
    angry,
    relaxed,
    none
};

// template<typename T>
// struct enum_identity { 
//   typedef T type; 
// };

// Emotion size(enum_identity<Emotion>) {
//     return none;
// }

struct States {
    int note_state{};
};

class Engine {
    private:
        static Engine * pinstance_;
        static std::mutex mutex_;

    protected:
        Engine();
        ~Engine() {}
    public:        
        std::map<std::string, Markov*> emotion_to_cadeia_notas;
        std::map<std::string, std::vector<int>> emotion_to_bpms;
        States states;
        std::atomic<Emotion> emotion;
        std::atomic<int> bpm;
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

        static Engine *GetInstance(std::vector<int> m);

        // std::string value() const{
        //     return value_;
        // } 
};



#endif //TCC_ENGINE_H