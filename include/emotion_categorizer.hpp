#ifndef TCC_EMOTIONCATEGORIZER_H
#define TCC_EMOTIONCATEGORIZER_H

#include <fstream>
#include <string>
#include "libjson/json.hpp"
#include "logger.hpp"
#include "json.hpp"
#include "database_manager.hpp"

class EmotionCategorizer {
    private:
    enum Emotions {
        happy,
        sad,
        tense,
        monotone
    };
    public:
        static void categorize() {
            using nlohmann::json;
            int categorized{};
            
            DatabaseManager* dbm = DatabaseManager::GetInstance();
            for (MidiFSEntry m_entry : dbm->database) {
                try {
                    json song_json;
                    std::filesystem::path json_path{m_entry.path};
                    json_path.replace_extension(".json");
                    Json::read_json(&song_json, json_path.string());

                    // abs(1-;2*(1-0.8));

                    song_json["audio_features"]["emotion"] = [](json feat_json)->std::string {
                        if (feat_json["valence"] > 0.5) return (feat_json["energy"] > 0.5 ? "happy" : "relaxed");
                        else return (feat_json["energy"] > 0.5 ? "angry" : "sad");
                    }(song_json["audio_features"]);

                    // features["emotion"] = {
                        // {"happy", (valence > 0.5 ? std::abs(1-2*(1-valence)) : 0)},
                        // {"sad", (valence < 0.5 ? std::abs(1-2*(1-valence)) : 0)},
                        // {"tense", (energy > 0.5 ? std::abs(1-2*(1-energy)) : 0)},
                        // {"monotone", (energy < 0.5 ? std::abs(1-2*(1-energy)) : 0)}
                    // };

                    std::fstream f;
                    f.open(json_path.string(), std::ios::out);
                    f << song_json.dump(4);
                    f.close();
                    categorized++;
                } catch (Json::unsuccesful_json_read const&) {
                    Logger::log(Logger::LOG_ERROR, "<Categorizer> Song JSON not found for %s, skipping...", m_entry.path.stem().c_str());
                }
            }
            Logger::log(Logger::LOG_ERROR, "<Categorizer> Categorized %d songs by emotion.", categorized);
        }
};

#endif //TCC_EMOTIONCATEGORIZER_H