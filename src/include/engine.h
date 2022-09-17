#ifndef TCC_ENGINE_H
#define TCC_ENGINE_H

#include "markov.h"
#include "stk_wrapper.h"
#include <mutex>

enum notas {
    c1 = 264, d = 297, e = 330, f = 352, g = 396, a = 440, b = 495, c2 = 528
}; 

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
        cadeia_notas(m_notas)
        {}
        ~Engine() {}

    public:
        Markov cadeia_notas;
        StkWrapper wrapper;
        void get_nota();
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
Engine *Engine::GetInstance(std::vector<int> m)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr)
    {
        pinstance_ = new Engine(m);
    }
    return pinstance_;
}

#endif //TCC_ENGINE_H