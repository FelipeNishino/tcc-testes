#ifndef TCC_REQUESTMANAGER_H
#define TCC_REQUESTMANAGER_H

#include <string>
#include <chrono>
#include <vector>
#include "libcurlpp/libcurlpp.h"

static const std::string SPOTIFY_ACCOUNT_URL = "https://accounts.spotify.com/api";
static const std::string SPOTIFY_BASE_URL = "https://api.spotify.com/v1";

class RequestManager {
    private:
        std::string authorization;
        std::string auth_token;
        std::chrono::time_point<std::chrono::system_clock> auth_expiration;
        void retrieve_auth_token();
        void read_song_list();
    public:
        RequestManager();
        ~RequestManager();
        void perform_request(curlpp::Easy *req);
        void request_track_feature_by_id(std::string song_id);
        void request_track_feature_by_ids(std::vector<std::string> song_id={});

};

#endif //TCC_REQUESTMANAGER_H