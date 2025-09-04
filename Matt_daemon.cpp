#include "Matt_daemon.hpp"
#include <iostream>
#include <signal.h>

MattDaemon::MattDaemon() {
    number_of_users = 0;
    server_socket = -1;  // Initialize to invalid value FIRST
    port = 4242;
    fd_flock = -1;       // Example port number
    max_number_of_users = 3; // Example maximum number of users
    lock = false;
    
    // Create socket AFTER initialization
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
        
    if (server_socket < 0) {
        std::cerr << "Erreur création socket dans constructeur" << std::endl;
    }
}

MattDaemon::~MattDaemon() {
}

void MattDaemon::start() {    
    if (server_socket == -1) {
        std::cerr << "Erreur lors de la création du socket" << std::endl;
        return;
    }
    
    // Permettre la réutilisation de l'adresse
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Erreur setsockopt");
        return;
    }
    
    // Configurer l'adresse
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    setfd_flock(open("/var/lock/matt_daemon.lock", O_RDWR | O_CREAT, 0666));
   /* if (getFdFlock() < 0) {
        reporter.report("- Matt_daemon: Error opening lock file.", "ERROR");
        perror("Erreur open");
        return;
    }*/
    if (flock(getFdFlock(), LOCK_EX | LOCK_NB) < 0) {
        setLock(true);
       reporter.report("- Matt_daemon: Error file locked.", "ERROR");
       reporter.report("- Matt_daemon: Quitting.", "INFO");
       return;
    }
    // Lier le socket à l'adresse
    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Erreur bind");
        std::cerr << "Erreur lors du bind sur le port " << port << std::endl;
        return;
    }
    
    // Commencer à écouter
    if (listen(server_socket, 5) < 0) {
        perror("Erreur listen");
        std::cerr << "Erreur lors de l'écoute" << std::endl;
        return;
    }

    reporter.report("- Matt_daemon: Started.", "INFO");
    reporter.report("- Matt_daemon: Creating server.", "INFO");
    reporter.report("- Matt_daemon: Server created.", "INFO");
    reporter.report("- Matt_daemon: Entering Daemon mode.", "INFO");
    reporter.report("- Matt_daemon: started. PID: " + std::to_string(getpid()) + ".", "INFO");
}

bool MattDaemon::handleClient(int client_socket) {
    char buffer[1024] = {0};
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    
    if (bytes_read <= 0) {
        reporter.report("- Matt_daemon: Signal handler.", "INFO");
        close(client_socket);
        reporter.report("- Matt_daemon: Quitting.", "INFO");
        number_of_users--;
        return false; 
    }
    
    // Nettoyer le buffer
    if (bytes_read > 0 && buffer[bytes_read - 1] == '\n') {
        buffer[bytes_read - 1] = '\0';
        bytes_read--;
    }
    if (bytes_read > 0 && buffer[bytes_read - 1] == '\r') {
        buffer[bytes_read - 1] = '\0';
    }
    
    buffer[bytes_read] = '\0'; // Null-terminate the string
    reporter.report("- Matt_daemon: User input: " + std::string(buffer), "LOG");
    
    if (strcmp(buffer, "quit") == 0) {
        reporter.report("- Matt_daemon: Request quit.", "INFO");
        close(client_socket);
        number_of_users--;
        reporter.report("- Matt_daemon: Quitting.", "INFO");
        kill(getpid(), SIGTERM); // Send SIGTERM to self
        return false;
    }
    
    return true; // Continuer à écouter ce client
}

void MattDaemon::stop() {
    reporter.report("- Matt_daemon: Stopping.", "INFO");
}
