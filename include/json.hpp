#ifndef TCC_JSON_H
#define TCC_JSON_H

#include <exception>
#include <string>
#include "libjson/json.hpp"
#include <fstream>

namespace Json {
    struct unsuccesful_json_read : public std::exception {
        std::string err_msg;
        public:
        unsuccesful_json_read(std::string path)
        : err_msg(std::string("Couldn't load json with path ") + path)
        {}
        const char * what () const throw () {
            return err_msg.c_str();
        }
    };

    
    
    static void read_json(nlohmann::json* json, std::string path) {
        std::fstream f;

        f.open(path, std::ios::in);
        if (f.fail())
            throw unsuccesful_json_read(path);
        else
            *json = nlohmann::json::parse(f);
        f.close();
    }
}

#endif //TCC_JSON_H