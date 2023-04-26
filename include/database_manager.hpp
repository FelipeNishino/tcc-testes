#ifndef TCC_DATABASE_MANAGER_H
#define TCC_DATABASE_MANAGER_H

#include <filesystem>
#include <iostream>
#include <string>
#include <mutex>
#include "libjson/json.hpp"
#include <vector>

struct data_dir_path_empty : public std::exception {
   	const char * what () const throw () {
		return "Data directory isn't set";
   	}
};

struct MidiFSEntry {
    std::string filename;
    std::filesystem::path path;
    bool has_features = false;
    std::string spotify_id() {
        return filename.substr(0, 22);
    }
    std::string json_path() {
        return std::filesystem::path{path}.replace_extension(".json");;
    }
    void dump() {
        std::cout << "Midi description:" << "\n";
        std::cout << "Filename: " << filename << "" << "\n";
        std::cout << "Path: " << path.string() << "" << "\n";
        std::cout << "Id: " << spotify_id() << "" << "\n";
        std::cout << "Features: " << (has_features ? "Yes" : "No") << "\n\n";
    }
};
        
class DatabaseManager {
    private:
        static DatabaseManager * pinstance_;
        static std::mutex mutex_;
        static inline std::string data_dir = "data/";
        void assert_data_dir_path();
        void import_files();
        bool verify_midi_filename(std::string filename);
        void load_files_from_data_dir();
    protected:
        DatabaseManager();
        ~DatabaseManager() {}
    public:
        void reload();
        static void set_data_dir(std::string path) { data_dir = path; }
        static std::string get_data_dir() { return data_dir; }
        static DatabaseManager *GetInstance();
        std::vector<MidiFSEntry> database;
        void save_features(std::vector<MidiFSEntry> no_feat_midis, std::vector<nlohmann::json> result_jsons);
        // Singletons should not be cloneable
        DatabaseManager(DatabaseManager &other) = delete;
        // Singletons should not be assignable.
        void operator=(const DatabaseManager &) = delete;
};

#endif //TCC_DATABASE_MANAGER_H