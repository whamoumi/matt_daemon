#include "Tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter() {
    reporter_path = "/var/log/matt_daemon/matt_daemon.log";

}

Tintin_reporter::~Tintin_reporter() {
    remove(reporter_path.c_str()); // Remove log file on destruction
}

void Tintin_reporter::report(const std::string& message, const std::string& level) {
    std::ofstream log_file(reporter_path, std::ios::app);
    if (log_file.is_open()) {
        log_file << "[ " << level << " ] " << message << std::endl;
        log_file.close();
    } else {
        std::cerr << "Failed to open log file: " << reporter_path << std::endl;
    }
}
