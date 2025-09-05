#include "Tintin_reporter.hpp"

// Constructeur par défaut
Tintin_reporter::Tintin_reporter() {
    reporter_path = "/var/log/matt_daemon/matt_daemon.log";
    reporter_lock = "";
}

// Constructeur de copie
Tintin_reporter::Tintin_reporter(const Tintin_reporter& other) 
    : reporter_path(other.reporter_path),
      reporter_lock(other.reporter_lock) {
    // Note: lock_file ne peut pas être copié car c'est un ofstream
    // Il sera initialisé si nécessaire lors de l'utilisation
}

// Opérateur d'assignation
Tintin_reporter& Tintin_reporter::operator=(const Tintin_reporter& other) {
    if (this != &other) {
        // Fermer le fichier existant s'il est ouvert
        if (lock_file.is_open()) {
            lock_file.close();
        }
        
        // Copier les données
        reporter_path = other.reporter_path;
        reporter_lock = other.reporter_lock;
        
        // Note: lock_file sera réinitialisé si nécessaire lors de l'utilisation
    }
    return *this;
}

// Destructeur
Tintin_reporter::~Tintin_reporter() {
    // Fermer le fichier s'il est ouvert
    if (lock_file.is_open()) {
        lock_file.close();
    }
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