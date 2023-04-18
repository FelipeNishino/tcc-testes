#include "database_manager.hpp"
#include "logger.hpp"

DatabaseManager* DatabaseManager::pinstance_{nullptr};
std::mutex DatabaseManager::mutex_;

/**
 * The first time we call GetInstance we will lock the storage location
 *      and then we make sure again that the variable is null and then we
 *      set the value. RU:
 */

DatabaseManager::DatabaseManager() {
    database = get_files_from_data_dir();
}

DatabaseManager *DatabaseManager::GetInstance() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pinstance_ == nullptr) {
        pinstance_ = new DatabaseManager();
    }
    return pinstance_;
}

bool DatabaseManager::verify_midi_filename(std::string filename) {
// int underscore_count = 0;
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

void DatabaseManager::assert_data_dir_path() {
    if (data_dir.empty()) throw data_dir_path_empty(); 
}

std::vector<MidiFSEntry> DatabaseManager::get_files_from_data_dir() {
    namespace fs = std::filesystem;
    std::vector<fs::directory_entry> genres;
    std::vector<MidiFSEntry> midi_files;
    assert_data_dir_path();

    Logger::log(Logger::LOG_DEBUG, "Tô aqui");

    for(const fs::directory_entry& entry: fs::directory_iterator{data_dir + "midi"}) {
        if (entry.is_directory()) {
            genres.push_back(entry);
        }
    }

    Logger::log(Logger::LOG_DEBUG, "Tô aqui");

    for (auto &genre : genres) {
        for(const fs::directory_entry& entry: fs::directory_iterator{genre.path()}) {
            if (entry.path().extension() != ".mid") {
                continue;
            }
            std::string filename = entry.path().filename().string();
            if (!verify_midi_filename(filename)) {
                Logger::log(Logger::LOG_ERROR, "Invalid Midi filename %s. Must have spotify id with trailing underscore at front", filename.c_str());
            }
            else {
                MidiFSEntry midi_file;
                midi_file.filename = filename;
                midi_file.path = entry.path();
                midi_files.push_back(midi_file);
                Logger::log(Logger::LOG_INFO, "Loaded %s.", filename.c_str());
            }
        }
    }
    return midi_files;
}