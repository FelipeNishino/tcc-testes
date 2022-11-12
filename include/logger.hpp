#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <iostream>
#include <ostream>
#include <stdarg.h>
#include <string>

class Logger {    
    public:
        enum LogLevel {
            LOG_FATAL,
            LOG_ERROR,
            LOG_WARNING,
            LOG_INFO,
            LOG_DEBUG
        };
        
        static void set_output_level(enum LogLevel level) { logger_level = level; }
		static std::string get_label_for_level(enum LogLevel level) {
			switch (level)
        	{
                case LOG_FATAL: return "Fatal: ";
                case LOG_ERROR: return "Error: ";
                case LOG_WARNING: return "Warning: ";
                case LOG_INFO: return "Info: ";
                case LOG_DEBUG: return "Debug: ";
                default: return "Internal Error!!!";
            }
		}
        static void log(enum LogLevel level, const char* format, ... ) {
			if (level > logger_level) return;            

			FILE* const output = (level <= LOG_WARNING) ? stderr : stdout;
			va_list args;
        	
			fprintf(output, "%s", get_label_for_level(level).c_str());
        	
        	va_start(args, format);
			vfprintf(output, format, args);
			va_end(args);
			fprintf(output, "\n" );
        }
        static void log(enum LogLevel level, std::string msg) {
        	std::ostream *output = (level <= LOG_WARNING) ? &std::cerr : &std::cout;
        	if (level > logger_level) return;            
            *output << get_label_for_level(level) << msg << '\n';
        }
    private:
        static inline enum LogLevel logger_level = LOG_ERROR;
};

#endif /*__LOGGER_H__*/