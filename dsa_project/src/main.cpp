#include <iostream>
#include <string>
#include <cstring>
#include "crypto.h"
#include <openssl/err.h>
#include <openssl/evp.h>

void print_usage() {
    std::cout <<
    "Usage:\n"
    "  fips186-dsa genkey -o <out-dir> [-b bits]\n"
    "  fips186-dsa sign -k <priv.pem> -i <infile> [-o <sigfile>] [--embed]\n"
    "  fips186-dsa verify -k <pub.pem> -i <infile> [-s <sigfile>] [--embedded]\n";
}

int main(int argc, char** argv) {
    if (argc < 2) { print_usage(); return 1; }
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    std::string cmd = argv[1];
    if (cmd == std::string("genkey")) {
        std::string outdir = ".";
        int bits = 2048;
        for (int i=2;i<argc;i++) {
            if (strcmp(argv[i], "-o")==0 && i+1<argc) { outdir = argv[++i]; }
            else if (strcmp(argv[i], "-b")==0 && i+1<argc) { bits = atoi(argv[++i]); }
        }
        std::string priv = outdir + "/dsa_private.pem";
        std::string pub  = outdir + "/dsa_public.pem";
        if (generate_dsa_keypair(priv, pub, bits)) {
            std::cout << "Generated keys: " << priv << " , " << pub << "\n";
            return 0;
        } else {
            std::cerr << "Key generation failed\n";
            return 2;
        }
    } else if (cmd == std::string("sign")) {
        std::string priv, infile, sigout;
        bool embed = false;
        for (int i=2;i<argc;i++) {
            if (strcmp(argv[i], "-k")==0 && i+1<argc) priv = argv[++i];
            else if (strcmp(argv[i], "-i")==0 && i+1<argc) infile = argv[++i];
            else if (strcmp(argv[i], "-o")==0 && i+1<argc) sigout = argv[++i];
            else if (strcmp(argv[i], "--embed")==0) embed = true;
        }
        if (priv.empty() || infile.empty()) { print_usage(); return 1; }
        if (!embed && sigout.empty()) {
            std::cerr << "Either --embed or -o <sigfile> required\n"; return 1;
        }
        if (sign_file_with_dsa(priv, infile, sigout, embed)) {
            std::cout << "Signed successfully\n";
            return 0;
        } else return 2;
    } else if (cmd == std::string("verify")) {
        std::string pub, infile, sigfile;
        bool embedded = false;
        for (int i=2;i<argc;i++) {
            if (strcmp(argv[i], "-k")==0 && i+1<argc) pub = argv[++i];
            else if (strcmp(argv[i], "-i")==0 && i+1<argc) infile = argv[++i];
            else if (strcmp(argv[i], "-s")==0 && i+1<argc) sigfile = argv[++i];
            else if (strcmp(argv[i], "--embedded")==0) embedded = true;
        }
        if (pub.empty() || infile.empty()) { print_usage(); return 1; }
        if (!embedded && sigfile.empty()) { std::cerr << "Specify -s <sigfile> or use --embedded\n"; return 1; }
        if (verify_file_with_dsa(pub, infile, sigfile, embedded)) return 0; else return 2;
    } else {
        print_usage();
        return 1;
    }
}
