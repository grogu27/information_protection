#include "../include/rsa_sign.h"
#include "../include/elgamal_sign.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <rsa|elgamal> <filename>\n";
        return 1;
    }

    std::string algo = argv[1];
    std::string file = argv[2];

    if (algo == "rsa") {
        rsa_sign_file(file);
        rsa_verify_file(file);
    } else if (algo == "elgamal") {
        elgamal_sign_file(file);
        elgamal_verify_file(file);
    } else {
        std::cerr << "Unknown algorithm. Use 'rsa' or 'elgamal'\n";
        return 1;
    }

    return 0;
}