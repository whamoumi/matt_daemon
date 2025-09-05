#include "Matt_daemon.hpp"
#include <iostream>
#include <vector>
#include <sys/select.h>
#include <algorithm>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>


int running = 1;

void handle_signal(int signal) {
    std::cout << "Received signal: " << signal << std::endl;
    if (signal == SIGTERM || signal == SIGINT || signal == SIGHUP || signal == SIGQUIT || signal == SIGKILL || signal == SIGCONT) {
        running = 0;
    }
}

void mainn() {
    MattDaemon daemon;
    daemon.start();

    std::vector<int> client_sockets;
    fd_set readfds;

    while (running) {

        if (daemon.isLocked()) {
            std::cout << "Daemon is locked, exiting." << std::endl;
            kill(0, SIGTERM);
        }
        int server_fd = daemon.getServerSocket();
        if (server_fd < 0) {
            std::cerr << "Socket serveur invalide: " << server_fd << std::endl;
            break;
        }
        
        FD_ZERO(&readfds);
        int max_fd = server_fd;
        FD_SET(server_fd, &readfds);
        for (size_t i = 0; i < client_sockets.size(); i++) {
            int client_fd = client_sockets[i];
            if (client_fd > 0) {
                FD_SET(client_fd, &readfds);
                if (client_fd > max_fd) {
                    max_fd = client_fd;
                }
            }
        }

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        
        if (activity < 0) {
            perror("Erreur select()");
            break;
        }
        
        if (activity == 0) {
            continue;
        }

        if (FD_ISSET(server_fd, &readfds)) {
            struct sockaddr_in client_address;
            socklen_t client_address_length = sizeof(client_address);
            int client_socket = accept(server_fd,
                                       (struct sockaddr*)&client_address,
                                       &client_address_length);

            if (client_socket >= 0) {
                if (daemon.number_of_users < daemon.max_number_of_users) {
                    client_sockets.push_back(client_socket);
                    daemon.number_of_users++;
                } else {
                    send(client_socket, "Server full. Try again later.\n", 30, 0);
                    close(client_socket);
                }
            }
        }

        for (size_t i = 0; i < client_sockets.size(); ) {
            int client_fd = client_sockets[i];

            if (client_fd > 0 && FD_ISSET(client_fd, &readfds)) {
                bool keep_connection = daemon.handleClient(client_fd);

                if (!keep_connection) {
                    close(client_fd);
                    client_sockets.erase(client_sockets.begin() + i);
                    daemon.number_of_users--;
                    continue;
                }
            }

            i++;
        }
    }
    daemon.reporter.report("- Matt_daemon: Signal handler", "INFO");
    daemon.reporter.report("- Matt_daemon: Quitting", "INFO");
    flock(daemon.getFdFlock(), LOCK_UN);
    close(daemon.getFdFlock());
    remove("/var/lock/matt_daemon.lock");
}
std::string getCurrentTimestamp() {
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

int main() {
    if (getuid() != 0) {
        std::cerr << "Error: Matt_daemon must be run as root." << std::endl;
        std::cerr << "Usage: sudo ./MattDaemon" << std::endl;
        return 1;
    }

    pid_t pid;

    int fd = open("/var/lock/matt_daemon.lock", O_RDWR, 0666);
    if (fd > 0)
    {
        if (flock(fd, LOCK_EX | LOCK_NB) < 0) 
        {
            std::cerr << "Matt_daemon is already running." << std::endl;
            std::ofstream log_file("/var/log/matt_daemon/matt_daemon.log", std::ios::app);
            if (log_file.is_open()) {
                log_file << getCurrentTimestamp() << " [ INFO ] - Matt_daemon: Started." << std::endl;
                log_file << getCurrentTimestamp() << " [ ERROR ] - Matt_daemon: Error file locked." << std::endl;
                log_file << getCurrentTimestamp() << " [ INFO ] - Matt_daemon: Quitting." << std::endl;
                log_file.close();
            } else {
                std::cerr << "Failed to open log file: " << "/var/log/matt_daemon/matt_daemon.log" << std::endl;
            }
            return 1;
        }
    }
 
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGHUP, handle_signal);
    signal(SIGQUIT, handle_signal);
    signal(SIGKILL, handle_signal);

    for (int sig = 1; sig < 64; sig++) { // Ignorer tout les signaux
        if (sig != SIGTERM && sig != SIGINT && sig != SIGHUP && sig != SIGQUIT && sig != SIGKILL)
            signal(sig, SIG_IGN);
    }

    if (chdir("/") < 0) {
        exit(EXIT_FAILURE);
    }
    umask(0);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    mainn();
    return EXIT_SUCCESS;
}