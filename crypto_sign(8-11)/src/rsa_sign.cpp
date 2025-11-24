#include "../include/rsa_sign.h"
#include "../include/utils.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <fstream>
#include <iostream>

void save_rsa_keys(RSA* rsa) {
    // Сохраняем приватный ключ
    FILE* priv = fopen("private.pem", "wb");
    PEM_write_RSAPrivateKey(priv, rsa, nullptr, nullptr, 0, nullptr, nullptr);
    fclose(priv);

    // Сохраняем публичный ключ
    FILE* pub = fopen("public.pem", "wb");
    PEM_write_RSA_PUBKEY(pub, rsa);
    fclose(pub);
}

RSA* load_private_key() {
    FILE* priv = fopen("private.pem", "rb");
    RSA* rsa = PEM_read_RSAPrivateKey(priv, nullptr, nullptr, nullptr);
    fclose(priv);
    return rsa;
}

RSA* load_public_key() {
    FILE* pub = fopen("public.pem", "rb");
    RSA* rsa = PEM_read_RSA_PUBKEY(pub, nullptr, nullptr, nullptr);
    fclose(pub);
    return rsa;
}

void rsa_sign_file(const std::string& filename) {
    auto hash = sha256_file(filename);

    // Генерация ключей (если не существуют)
    RSA* rsa = RSA_generate_key(2048, RSA_F4, nullptr, nullptr);
    if (!rsa) {
        std::cerr << "RSA key generation failed\n";
        return;
    }
    save_rsa_keys(rsa);

    std::vector<unsigned char> signature;
    for (unsigned char b : hash) {
        unsigned char sig[256];
        unsigned int siglen;
        if (!RSA_sign(NID_sha256, &b, 1, sig, &siglen, rsa)) {
            std::cerr << "Signing byte failed\n";
            RSA_free(rsa);
            return;
        }
        signature.insert(signature.end(), sig, sig + siglen);
    }
    RSA_free(rsa);

    write_file(filename + "_sig", signature);
    std::cout << "RSA signature saved to " << filename + "_sig\n";
}

void rsa_verify_file(const std::string& filename) {
    auto hash = sha256_file(filename);
    auto sig_data = read_file(filename + "_sig");

    RSA* rsa = load_public_key();
    if (!rsa) {
        std::cerr << "Cannot load public key\n";
        return;
    }

    size_t pos = 0;
    bool ok = true;
    for (unsigned char b : hash) {
        unsigned int siglen = RSA_size(rsa);
        if (pos + siglen > sig_data.size()) {
            ok = false; break;
        }
        if (RSA_verify(NID_sha256, &b, 1, sig_data.data() + pos, siglen, rsa) != 1) {
            ok = false; break;
        }
        pos += siglen;
    }
    RSA_free(rsa);

    std::ofstream out(filename + "_ver");
    out << (ok ? "VALID" : "INVALID");
    std::cout << "RSA verification: " << (ok ? "VALID" : "INVALID") << "\n";
}