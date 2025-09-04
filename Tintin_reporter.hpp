#include <iostream>
#include <fstream>
#include <string>

class Tintin_reporter {
    public:
        Tintin_reporter();
        ~Tintin_reporter();

        void report(const std::string& message, const std::string& level);

    private:
        std::string reporter_path;
        std::string reporter_lock;
        std::ofstream lock_file;
};