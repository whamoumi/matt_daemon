#include "Tintin_reporter.hpp"

Tintin_reporter::Tintin_reporter() {
    reporter_path = "/var/log/matt_daemon/matt_daemon.log";

}

Tintin_reporter::~Tintin_reporter() {
    remove(reporter_path.c_str());
}

std::string Tintin_reporter::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    
    std::ostringstream oss;
    oss << "[" 
        << std::setfill('0') << std::setw(2) << localTime->tm_mday << "/"
        << std::setfill('0') << std::setw(2) << (localTime->tm_mon + 1) << "/"
        << (localTime->tm_year + 1900) << "-"
        << std::setfill('0') << std::setw(2) << localTime->tm_hour << ":"
        << std::setfill('0') << std::setw(2) << localTime->tm_min << ":"
        << std::setfill('0') << std::setw(2) << localTime->tm_sec
        << "]";
    
    return oss.str();
}

void Tintin_reporter::report(const std::string& message, const std::string& level) {
    std::ofstream log_file(reporter_path, std::ios::app);
    
    if (log_file.is_open()) {
        log_file << getCurrentTimestamp() << " [ " << level << " ] " << message << std::endl;
        log_file.close();
    } else {
        std::cerr << "Failed to open log file: " << reporter_path << std::endl;
    }
}