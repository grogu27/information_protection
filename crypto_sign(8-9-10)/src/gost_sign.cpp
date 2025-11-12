// src/gost_sign.cpp — КОРРЕКТНАЯ ВЕРСИЯ
#include "../include/gost_sign.h"
#include "../include/utils.h"
#include "gost341094.h"
#include <iostream>

using namespace std;

void gost_sign_and_verify_file(const string& filename) {
    GOST341094 gost;
    gost.generate_keys();
    
    auto hash = sha256_file(filename);
    auto sig = gost.sign(hash);
    string sigFile = filename + ".gost_sig";
    gost.save_signature(sig, sigFile);
    cout << "✓ Signed → " << sigFile << endl;

    auto loaded_sig = gost.load_signature(sigFile);
    if (gost.verify(hash, loaded_sig)) {
        cout << "✓ Verified OK" << endl;
    } else {
        throw runtime_error("Verification failed");
    }
}