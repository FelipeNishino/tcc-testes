#include <cerrno>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include "libjson/json.hpp"
#include "encoder.hpp"
#include "logger.hpp"
#include "request_manager.hpp"
#include "database_manager.hpp"

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

bool RequestManager::request_track_features() {
		using namespace curlpp::Options;
		using std::string;
        DatabaseManager* dbm = DatabaseManager::GetInstance();
		std::vector<string> song_list{};
		string buf;
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
        std::vector<MidiFSEntry> no_feat_midis;

        std::copy_if(
            dbm->database.begin(),
            dbm->database.end(),
            std::back_inserter(no_feat_midis),
            [](MidiFSEntry m){return !m.has_features;}
        );

        if (no_feat_midis.empty()) {
            return false;
        }
        auto it_songs = no_feat_midis.begin();
        while (true) {
            if (song_list.size() < SPOTIFY_MAX_TRACK_REQ && it_songs != no_feat_midis.end()) {
                song_list.push_back(it_songs->spotify_id());
                it_songs++;
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
				if (it_songs == no_feat_midis.end()) break;
			}
        }
		dbm->save_features(no_feat_midis, result_jsons);
		
        Logger::log(Logger::LOG_INFO, "Requisições a Spotify API realizadas com successo");
        return true;
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
		// std::cout << "Intervalo: " << interval.count() << "\n";
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
