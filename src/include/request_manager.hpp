#ifndef TCC_REQUESTMANAGER_H
#define TCC_REQUESTMANAGER_H

#include <string>
#include "libcurlpp/libcurlpp.h"
#include <chrono>

class RequestManager {
    private:
        std::string authorization;
        std::string auth_token;
        std::chrono::seconds auth_expiration;
    public:
        RequestManager();
        void perform_request();
};

#endif //TCC_REQUESTMANAGER_H