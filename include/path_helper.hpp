#ifndef TCC_PATH_HELPER_H
#define TCC_PATH_HELPER_H

#include <string>

class PathHelper {
    public:
        static inline std::string RUN_PATH = "";
        static void set_run_path(std::string path) { RUN_PATH = path; }
};

#endif //TCC_PATH_HELPER_H