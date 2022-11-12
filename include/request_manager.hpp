#ifndef TCC_REQUESTMANAGER_H
#define TCC_REQUESTMANAGER_H

#include <chrono>
#include <string>
#include <vector>
#include "libcurlpp/libcurlpp.h"


class RequestManager {
    private:
        static inline const std::string SPOTIFY_ACCOUNT_URL = "https://accounts.spotify.com/api";
        static inline const std::string SPOTIFY_BASE_URL = "https://api.spotify.com/v1";
        static inline const unsigned int SPOTIFY_MAX_TRACK_REQ = 100;

        std::string authorization;
        std::string auth_token;
        std::chrono::time_point<std::chrono::system_clock> auth_expiration;
        void retrieve_auth_token();
        void read_song_list();
    public:
        RequestManager();
        ~RequestManager();
        static unsigned int flags;
        void perform_request(curlpp::Easy *req);
        void request_track_feature_from_list(std::string filename="data/song_list.txt");
        void request_track_feature_by_id(std::vector<std::string> song_id={});
};

#endif //TCC_REQUESTMANAGER_H