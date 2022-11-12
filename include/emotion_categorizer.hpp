#ifndef TCC_EMOTIONCATEGORIZER_H
#define TCC_EMOTIONCATEGORIZER_H

#include <fstream>
#include <string>
#include "libjson/json.hpp"
#include "logger.hpp"

class EmotionCategorizer {
    private:
    enum Emotions {
        happy,
        sad,
        tense,
        monotone
    };
    public:
        static void categorize(std::string filename="data/features.json") {
            using nlohmann::json;
            std::fstream f;
            json features_json;
            f.open(filename, std::ios::in);
            if (f.fail())
                Logger::log(Logger::LOG_ERROR, "Couldn't open file %s", filename.c_str());
            else
                features_json = json::parse(f);         
            
            f.close();
            
            for (auto &features : features_json["audio_features"]) {
                // abs(1-;2*(1-0.8));
                features["emotion"] = [features]()->std::string{
                    if (features["valence"] > 0.5) return (features["energy"] > 0.5 ? "happy" : "relaxed");
                    else return (features["energy"] > 0.5 ? "angry" : "sad");
                }();
                // features["emotion"] = {
                    // {"happy", (valence > 0.5 ? std::abs(1-2*(1-valence)) : 0)},
                    // {"sad", (valence < 0.5 ? std::abs(1-2*(1-valence)) : 0)},
                    // {"tense", (energy > 0.5 ? std::abs(1-2*(1-energy)) : 0)},
                    // {"monotone", (energy < 0.5 ? std::abs(1-2*(1-energy)) : 0)}
                // };
            }
            
            f.open("data/features2.json", std::ios::out);
            f << features_json.dump(4);
            f.close();
        }
};

#endif //TCC_EMOTIONCATEGORIZER_H