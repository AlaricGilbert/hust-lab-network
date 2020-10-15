#include "logger.h"
#include "string_utils.h"

Logger::Logger(const std::initializer_list<std::ostream*>& outstreams) {
    ostreams = outstreams;
}

void Logger::info(const std::string& info) {
    log("INFO", info);
}
void Logger::warn(const std::string& warn) {
    log("WARN", warn);
}
void Logger::error(const std::string& error) {
    log("ERROR", error);
}
void Logger::log(const std::string& clazz, const std::string& log_info) {
    for (auto &outstream:ostreams)
    {
        *outstream << "[" << get_time_string() << "][" << clazz << "]" << log_info << std::endl;
    }
}