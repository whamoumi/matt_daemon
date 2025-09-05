#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>

class Tintin_reporter {
    public:
        // Forme canonique de Coplien
        Tintin_reporter();                                        // Constructeur par défaut
        Tintin_reporter(const Tintin_reporter& other);           // Constructeur de copie
        Tintin_reporter& operator=(const Tintin_reporter& other); // Opérateur d'assignation
        ~Tintin_reporter();                                       // Destructeur

        void report(const std::string& message, const std::string& level);

    private:
        std::string reporter_path;
        std::string reporter_lock;
        std::ofstream lock_file;
        std::string getCurrentTimestamp();
};