#ifndef TCC_DATABASE_MANAGER_H
#define TCC_DATABASE_MANAGER_H

#include <filesystem>
#include <string>
#include <mutex>
#include <vector>

struct data_dir_path_empty : public std::exception {
   	const char * what () const throw () {
		return "Data directory isn't set";
   	}
};

struct MidiFSEntry {
    std::string filename;
    std::filesystem::path path;
    std::string spotify_id() {
        return filename.substr(0, 22);
    }
};
        
class DatabaseManager {
    private:
        static DatabaseManager * pinstance_;
        static std::mutex mutex_;
        static inline std::string data_dir = "data/";
        void assert_data_dir_path();
        bool verify_midi_filename(std::string filename);
        std::vector<MidiFSEntry> get_files_from_data_dir();
    protected:
        DatabaseManager();
        ~DatabaseManager() {}
    public:
        static void set_data_dir(std::string path) { data_dir = path; }
        static DatabaseManager *GetInstance();
        std::vector<MidiFSEntry> database;

        // Singletons should not be cloneable
        DatabaseManager(DatabaseManager &other) = delete;
        // Singletons should not be assignable.
        void operator=(const DatabaseManager &) = delete;
};

#endif //TCC_DATABASE_MANAGER_H