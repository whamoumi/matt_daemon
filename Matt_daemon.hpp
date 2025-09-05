#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "Tintin_reporter.hpp"
#include <fcntl.h>
#include <sys/file.h>  
#include <sys/stat.h>
#include <cstdlib>
class MattDaemon {
    public:
        // Forme canonique de Coplien
        MattDaemon();                                   // Constructeur par défaut
        MattDaemon(const MattDaemon& other);           // Constructeur de copie
        MattDaemon& operator=(const MattDaemon& other); // Opérateur d'assignation
        ~MattDaemon();                                  // Destructeur

        Tintin_reporter reporter;
        unsigned int number_of_users;
        unsigned int max_number_of_users;
        void start();
        bool isLocked() const { return lock; }
        void setLock(bool l) { lock = l; }
        void setfd_flock(int fd) { fd_flock = fd; }
        int getFdFlock() const { return fd_flock; }
        int getServerSocket() const { return server_socket; }
        bool handleClient(int client_socket);
        void stop();
    private:
        bool lock;
        int server_socket;
        int fd_flock;
        struct sockaddr_in address;
        int port;
};