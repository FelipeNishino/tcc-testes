#ifndef TCC_EMOTION_HPP
#define TCC_EMOTION_HPP

#include <vector>
#include <atomic>
#include <string>

struct Emotion {
    public:
        static inline const std::vector<std::string> EMO_TO_STR = {"happy", "sad", "angry", "relaxed"};
        enum emotions {
            happy,
            sad,
            angry,
            relaxed,
        };
        void operator=(const emotions &other) {e.store(other);};
        std::string str() {return EMO_TO_STR[e.load()];}
    private:
        std::atomic<emotions> e;    
};

#endif //TCC_EMOTION_HPP    