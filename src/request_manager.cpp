#include "request_manager.hpp"
#include "encoder.hpp"
#include <chrono>
#include <exception>
#include <cstdlib>
#include <ostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <cerrno>
#include <fstream>
#include "libcurlpp/curlpp/Infos.hpp"
#include "libjson/json.hpp"
#include "logger.hpp"

using std::chrono::seconds;
using std::chrono::duration_cast;

unsigned int RequestManager::flags = 0;

struct no_environment_vars : public std::exception {
   	const char * what () const throw () {
		return "Did not find needed environment variables.";
   	}
};

void RequestManager::retrieve_auth_token() {
	try {
    	curlpp::Easy request;
		using namespace curlpp::Options;
		nlohmann::json result_json;
		std::list<std::string> header {
			"Content-Type: application/x-www-form-urlencoded",
			"Authorization: " + authorization
		}; 
		std::string param = "grant_type=client_credentials";
		std::ostringstream result_stream;	
		std::vector<curlpp::OptionBase *> opt{
			new Url(SPOTIFY_ACCOUNT_URL + "/token"),
			new Verbose(false),
			new HttpHeader(header),
			new PostFields(param),
			new PostFieldSize(param.size()),
			new WriteStream(&result_stream)
		};
		
		request.setOpt(opt.begin(), opt.end());

    	request.perform();
		long response_code = curlpp::infos::ResponseCode::get(request);
		switch (response_code) {
			case 200 ... 299:
				std::cout << "Retrieved auth token: " << response_code << '\n';
				result_json = nlohmann::json::parse(result_stream.str());
				auth_token = "Bearer " + std::string(result_json["access_token"]);
				auth_expiration = std::chrono::system_clock::now();
				break;
			case 400 ... 499:
				std::cout << "Request error: " << response_code << '\n';
				break;
			case 500 ... 599:
				std::cout << "Server error" << response_code << '\n';
				break;		
			default:
				break;
		}
  	}
	catch ( curlpp::LogicError & e ) {
		std::cout << e.what() << std::endl;
	}
	catch ( curlpp::RuntimeError & e ) {
		std::cout << e.what() << std::endl;
	}
}

RequestManager::RequestManager() {
	std::string client_id;
	std::string client_secret;

	curlpp::initialize();

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

RequestManager::~RequestManager() {
	cURLpp::terminate();
}

void RequestManager::request_track_feature_from_list(std::string filename) {
		using namespace curlpp::Options;
		using std::string;

		std::vector<string> song_list{};
		string buf;
		std::fstream file(filename, std::ios::in);
		curlpp::Easy request;
		nlohmann::json f_json;
		std::vector<nlohmann::json> result_jsons;
		std::list<std::string> header {
			"Content-Type: application/json",
			"Authorization: " + auth_token,
		};
		std::ostringstream result_stream;
		std::string ids;
		std::vector<curlpp::OptionBase *> opt{
			new HttpHeader(header),
			new HttpGet(true),
			new WriteStream(&result_stream)
		};
		request.setOpt(opt.begin(), opt.end());

		if (file.fail()) {
			std::cout << "deu ruim" << std::endl;
        	file.close();
    	}

		// TODO: Verificar se a lista está vazia

		while (true) {
			if (song_list.size() < SPOTIFY_MAX_TRACK_REQ && !file.eof()) {
				getline(file, buf);
				if (buf.empty()) continue;
				song_list.push_back(buf.substr(buf.rfind("|") + 1));
			}
			else {
				if (!song_list.empty()) {
					for (std::string &s : song_list) {
						ids.append(s + ",");	
					}
					request.setOpt(new Url(SPOTIFY_BASE_URL + "/audio-features?ids=" + ids));
					perform_request(&request);
					song_list.clear();
					ids.clear();
					long response_code = curlpp::infos::ResponseCode::get(request);
					switch (response_code) {
						case 200 ... 299:
							std::cout << "\n Sucesso: " << response_code << '\n';
							result_jsons.push_back(nlohmann::json::parse(result_stream.str()));
							result_stream.clear();
							result_stream.str("");
							// std::cout << "Features recebidos: " << (result_json["audio_features"].size()) << '\n';
							break;
						case 400 ... 499:
							std::cout << "\n Request error: " << response_code << '\n';
							break;
						case 500 ... 599:
							std::cout << "\n Server error" << response_code << '\n';
							break;		
						default:
							break;
					}
				}
				if (file.eof()) break;
			}
		}
		for (auto it = result_jsons.begin() + 1; it != result_jsons.end(); it++) {
			result_jsons[0]["audio_features"].push_back((*it)["audio_features"]);
		}
		
		std::fstream output_file("data/features.json", std::ios::out);;
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
		// std::cout << "count json final: " << (result_jsons[0]["audio_features"].size()) << '\n';
		// std::cout << "dps closure " << result_jsons[0].dump(4) << '\n';
		// result_jsons[0]["audio_features"]
		output_file << result_jsons[0].dump(4);
		output_file.close();
        Logger::log(Logger::LOG_INFO, "Requisições a Spotify API realizadas com successo");
}

void RequestManager::request_track_feature_by_id(std::vector<std::string> song_id) {
		using namespace curlpp::Options;
    	
		// if (song_id.empty()) {
		// 	read_song_list();
		// }

		curlpp::Easy request;
		nlohmann::json result_json;
		std::list<std::string> header {
			"Content-Type: application/json",
			"Authorization: " + auth_token,
		}; 
		std::ostringstream result_stream;
		std::string ids;
		for (std::string &s : song_id) {
			ids.append(s + ",");	
		}
		std::vector<curlpp::OptionBase *> opt{
			new Url(SPOTIFY_BASE_URL + "/audio-features?ids=" + ids),
			new HttpHeader(header),
			new HttpGet(true),
			new WriteStream(&result_stream)
		};
		
		request.setOpt(opt.begin(), opt.end());

		perform_request(&request);
		
		long response_code = curlpp::infos::ResponseCode::get(request);
		switch (response_code) {
			case 200 ... 299:
				// std::cout << "Sucesso: " << response_code << '\n';
				result_json = nlohmann::json::parse(result_stream.str());
				// std::cout << "Features recebidos: " << (result_json["audio_features"]) << '\n';
				break;
			case 400 ... 499:
				std::cout << "Request error: " << response_code << '\n';
				// std::cout << result_stream.str() << '\n';
				// std::cout << curlpp::infos::EffectiveUrl::get(request);
				break;
			case 500 ... 599:
				std::cout << "Server error" << response_code << '\n';
				break;		
			default:
				break;
		}
}

// void RequestManager::request_track_feature_by_id(std::string song_id) {
// 	curlpp::Easy request;
// 	using namespace curlpp::Options;
// 	nlohmann::json result_json;
// 	std::list<std::string> header {
// 		"Content-Type: application/json",
// 		"Authorization: " + auth_token,
// 	}; 
// 	std::ostringstream result_stream;

// 	std::vector<curlpp::OptionBase *> opt{
// 		new Url(SPOTIFY_BASE_URL + "/audio-features/" + song_id),
// 		new HttpHeader(header),
// 		new HttpGet(true),
// 		// new WriteStream(&result_stream)
// 	};
	
// 	request.setOpt(opt.begin(), opt.end());

// 	perform_request(&request);

// 	long response_code = curlpp::infos::ResponseCode::get(request);
// 	switch (response_code) {
// 		case 200 ... 299:
// 			// std::cout << "Sucesso: " << response_code << '\n';
// 			// result_json = nlohmann::json::parse(result_stream.str());
// 			break;
// 		case 400 ... 499:
// 			std::cout << "Request error: " << response_code << '\n';
// 			break;
// 		case 500 ... 599:
// 			std::cout << "Server error" << response_code << '\n';
// 			break;		
// 		default:
// 			break;
// 	}
  	
// }

void RequestManager::perform_request(curlpp::Easy *req) {
	const auto interval = duration_cast<seconds>(auth_expiration.time_since_epoch());
	// TODO: N TA FUNFANDO
	if (interval.count() > 3600) {
		std::cout << "Intervalo: " << interval.count() << "\n";
		// retrieve_auth_token();
	}

	try {
    	req->perform();
  	}
	catch ( curlpp::LogicError & e ) {
		std::cout << e.what() << std::endl;
	}
	catch ( curlpp::RuntimeError & e ) {
		std::cout << e.what() << std::endl;
	}
}
