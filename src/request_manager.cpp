#include "include/request_manager.hpp"
#include "include/encoder.hpp"
#include <chrono>
#include <exception>
#include <cstdlib>
#include <stdexcept>
#include <vector>


struct no_environment_vars : public std::exception {
   	const char * what () const throw () {
		return "Did not find needed environment variables.";
   	}
};

void RequestManager::retrieve_auth_token() {
	
}

RequestManager::RequestManager() {
	std::string client_id;
	std::string client_secret;

	try {
		client_id = std::string(std::getenv("SPOTIFY_CLIENT_ID"));
		client_secret = std::string(std::getenv("SPOTIFY_CLIENT_SECRET"));
	} catch (const std::exception& e) {
		throw no_environment_vars();
	}
	
	std::string str = "";
	authorization = "Basic ";

	str.append(client_id);
	str.append(":");
	str.append(client_secret);

	authorization.append(Encoder::encode(str, base64));

	retrieve_auth_token();
}

void RequestManager::perform_request() {
    const auto p1 = std::chrono::system_clock::now();

    std::cout << "seconds since epoch: "
              << std::chrono::duration_cast<std::chrono::seconds>(
                   p1.time_since_epoch()).count() << '\n';
}

