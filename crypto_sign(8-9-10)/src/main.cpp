#include "../include/rsa_sign.h"
#include "../include/elgamal_sign.h"
#include "../include/gost_sign.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <rsa|elgamal|gost> <filename>\n";
        std::cerr << "Examples:\n";
        std::cerr << "  " << argv[0] << " rsa document.txt\n";
        std::cerr << "  " << argv[0] << " elgamal data.bin\n";
        std::cerr << "  " << argv[0] << " gost file.pdf\n";
        return 1;
    }

    std::string algo = argv[1];
    std::string file = argv[2];

    try {
        if (algo == "rsa") {
            rsa_sign_file(file);
            rsa_verify_file(file);
        } 
        else if (algo == "elgamal") {
            elgamal_sign_file(file);
            elgamal_verify_file(file);
        }
        else if (algo == "gost") {
            gost_sign_file(file);
            gost_verify_file(file);
        }
        else {
            std::cerr << "Unknown algorithm. Use 'rsa', 'elgamal' or 'gost'\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}