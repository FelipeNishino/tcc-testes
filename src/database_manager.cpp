#include "database_manager.hpp"
#include "request_manager.hpp"
#include "emotion_categorizer.hpp"
#include "logger.hpp"
#include "utils.hpp"

DatabaseManager* DatabaseManager::pinstance_{nullptr};
std::mutex DatabaseManager::mutex_;

/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value. RU:
 */
DatabaseManager::DatabaseManager() {
    import_files();
    load_files_from_data_dir();
}

DatabaseManager *DatabaseManager::GetInstance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr) {
        pinstance_ = new DatabaseManager();
    }
    return pinstance_;
}

void DatabaseManager::reload() {
    load_files_from_data_dir();
}

void DatabaseManager::assert_data_dir_path() {
    if (data_dir.empty()) throw data_dir_path_empty(); 
}

void DatabaseManager::import_files() {
    std::filesystem::directory_entry d{ "import/" };

    if (d.exists() && d.is_directory()) {
        if (std::distance(std::filesystem::directory_iterator{d}, std::filesystem::directory_iterator{}) == 0) {
            Logger::log(Logger::LOG_INFO, "<Database> No songs to import.");
            return;
        }
        for (auto const& entry : std::filesystem::directory_iterator{d}) {
            if (entry.path().extension() != ".mid") continue;
            std::string filename = entry.path().filename().string();

            if (!verify_midi_filename(filename)) {
                Logger::log(Logger::LOG_ERROR, "<Database> Invalid Midi filename %s. Name must start with spotify id with trailing underscore", filename.c_str());
            }
            else {
                MidiFSEntry midi_file;
                midi_file.filename = filename;
                midi_file.path = entry.path();

                std::filesystem::path new_path{"data/" + midi_file.spotify_id()};

                if (!std::filesystem::create_directories(new_path)) {
                    Logger::log(Logger::LOG_WARNING, "<Database> Couldn't create directory data/%s.", midi_file.spotify_id().c_str());    
                    continue;
                }
                
                std::filesystem::copy(midi_file.path, std::filesystem::path{new_path.relative_path().string() + "/" + filename});
                std::filesystem::remove(midi_file.path);
                Logger::log(Logger::LOG_INFO, "<Database> Created data entry for %s.", filename.c_str());
            }
        }
    }
}

bool DatabaseManager::verify_midi_filename(std::string filename) {
    Logger::log(Logger::LOG_DEBUG, "Checking entry: %s", filename.c_str());
    Logger::log(Logger::LOG_DEBUG, "Checking size: %d", filename.size());
    if (filename.size() < 23) return false;
    Logger::log(Logger::LOG_DEBUG, "Checking underscore");
    if (filename.at(22) != '_') return false;
    Logger::log(Logger::LOG_DEBUG, "Checking id");
    for (int i = 0; i < 22; i++) {
        if (filename[i] == '_') return false;
    }
    Logger::log(Logger::LOG_DEBUG, "Ok");
    return true;
}

void DatabaseManager::load_files_from_data_dir() {
    namespace fs = std::filesystem;

    std::vector<MidiFSEntry> midi_files;
    assert_data_dir_path();
    
    for(const fs::directory_entry& song_dir: fs::directory_iterator{data_dir}) {
        if (!song_dir.is_directory()) continue;

        for(const fs::directory_entry& entry: fs::directory_iterator{song_dir}) {
            if (entry.path().extension() != ".mid") continue;
            std::string filename = entry.path().filename().string();

            if (!verify_midi_filename(filename)) {
                Logger::log(Logger::LOG_ERROR, "<Database> Invalid Midi filename %s. Name must start with spotify id with trailing underscore", filename.c_str());
            }
            else {
                MidiFSEntry midi_file;
                midi_file.filename = filename;
                midi_file.path = entry.path();
                midi_files.push_back(midi_file);
                if (std::distance(fs::directory_iterator{song_dir}, std::filesystem::directory_iterator{}) > 1) {
                    midi_file.has_features = true;
                }
                Logger::log(Logger::LOG_DEBUG, "<Database> Loaded %s.", filename.c_str());
            }
        }
    }
    Logger::log(Logger::LOG_INFO, "<Database> Loaded %d Midi files.", midi_files.size());
    database = midi_files;
}

void DatabaseManager::save_features(std::vector<MidiFSEntry> no_feat_midis, std::vector<nlohmann::json> result_jsons) {
        for (auto it = result_jsons.begin() + 1; it != result_jsons.end(); it++) {
			result_jsons[0]["audio_features"].push_back((*it)["audio_features"]);
		}
		
		std::fstream output_file("data/features.json", std::ios::out);
		if (output_file.fail()) {
			std::cout << "deu ruim" << std::endl;
        	output_file.close();
    	}
		int i{};
		while(true) {
			i = [result_jsons](int i)->int {
				int j{};
				for (auto feature : result_jsons[0]["audio_features"]) {
					if(feature.is_null()) return j;
					j++;
				}
				return -1;
			}(i);
			if (i >= 0) {
                std::cout << "ACHOU NULO NA MÚSICA " << i << "\n";
				result_jsons[0]["audio_features"].erase(i);
            }
			else
				break;
		}
        std::sort(
            result_jsons[0]["audio_features"].begin(),
            result_jsons[0]["audio_features"].end(),
            [](auto ja, auto jb) {return ja["id"] < jb["id"];}
        );
        
        std::sort(
            no_feat_midis.begin(),
            no_feat_midis.end(),
            [](MidiFSEntry ea, MidiFSEntry eb) {return ea.spotify_id() < eb.spotify_id();}
        );
		// std::cout << "count json final: " << (result_jsons[0]["audio_features"].size()) << '\n';
		// std::cout << "dps closure " << result_jsons[0].dump(4) << '\n';
		// result_jsons[0]["audio_features"]
        for_each_zipped_containers(
            [](nlohmann::json feat, MidiFSEntry midi_entry) {
                Logger::log(Logger::LOG_WARNING, "Tô aqui");

                nlohmann::json result = {"audio_features", feat};
                Logger::log(Logger::LOG_WARNING, "Vai tentar salvar em %s", midi_entry.path.replace_extension(".json").string().c_str());

                std::fstream output_file(midi_entry.path.replace_extension(".json").string(), std::ios::out);
                if (output_file.fail()) {
			        std::cout << "deu ruim" << std::endl;
        	        output_file.close();
    	        }
                output_file << result.dump(4);
		        output_file.close();
            },
            result_jsons[0]["audio_features"],
            no_feat_midis
        );
}