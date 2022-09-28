#include "include/request_manager.hpp"
#include "include/encoder.hpp"
#include <chrono>
#include <exception>
#include <stdlib.h>
#include <stdexcept>
#include <vector>


struct no_environment_vars : public std::exception {
   const char * what () const throw () {
      return "Did not find needed environment variables.";
   }
};


RequestManager::RequestManager() {
    std::string client_id = getenv("CLIENT_ID");
    std::string client_secret = getenv("CLIENT_SECRET");
    std::string str{};
    authorization = "Basic ";

    if ( client_id.empty() || client_secret.empty()) throw no_environment_vars();

    str.append(client_id);
    str.append(":");
    str.append(client_secret);
    
    authorization.append(Encoder::encode(str, base64))

}

void RequestManager::perform_request() {
    const auto p1 = std::chrono::system_clock::now();

    std::cout << "seconds since epoch: "
              << std::chrono::duration_cast<std::chrono::seconds>(
                   p1.time_since_epoch()).count() << '\n';
}

