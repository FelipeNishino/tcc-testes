#ifndef TCC_ENGINE_H
#define TCC_ENGINE_H

#include "markov.hpp"
#include "stk_wrapper.hpp"
#include "utils.hpp"
#include <mutex>

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

class Engine {
    private:
        static Engine * pinstance_;
        static std::mutex mutex_;

    protected:
        Engine(std::vector<int> m_notas):
        cadeia_notas(m_notas),default_octave(5),count_notas(0)
        {}
        ~Engine() {}

    public:
        
        Markov cadeia_notas;
        int default_octave;
        StkWrapper wrapper;
        int get_note();
        int count_notas;
        void play();

        // Singletons should not be cloneable
        Engine(Engine &other) = delete;
        // Singletons should not be assignable.
        void operator=(const Engine &) = delete;

        static Engine *GetInstance(std::vector<int> m);

        // std::string value() const{
        //     return value_;
        // } 
};



#endif //TCC_ENGINE_H