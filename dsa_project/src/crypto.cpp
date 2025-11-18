#include "crypto.h"
#include "utils.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/dsa.h>
#include <openssl/err.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>

static void print_openssl_errors() {
    unsigned long e;
    while ((e = ERR_get_error()) != 0) {
        char buf[256];
        ERR_error_string_n(e, buf, sizeof(buf));
        std::cerr << "OpenSSL error: " << buf << "\n";
    }
}

bool generate_dsa_keypair(const std::string& priv_out, const std::string& pub_out, int bits) {
    bool ok = false;
    DSA* dsa = DSA_new();
    if (!dsa) { print_openssl_errors(); return false; }

    // Generate parameters and key (this might take time for large sizes)
    if (!DSA_generate_parameters_ex(dsa, bits, NULL, 0, NULL, NULL, NULL)) {
        print_openssl_errors();
        DSA_free(dsa);
        return false;
    }

    if (!DSA_generate_key(dsa)) {
        print_openssl_errors();
        DSA_free(dsa);
        return false;
    }

    EVP_PKEY* pkey = EVP_PKEY_new();
    if (!pkey) { DSA_free(dsa); print_openssl_errors(); return false; }
    if (!EVP_PKEY_set1_DSA(pkey, dsa)) {
        print_openssl_errors();
        EVP_PKEY_free(pkey);
        DSA_free(dsa);
        return false;
    }

    // write private key
    FILE* fpriv = fopen(priv_out.c_str(), "wb");
    if (!fpriv) { std::cerr << "Cannot open " << priv_out << "\n"; EVP_PKEY_free(pkey); DSA_free(dsa); return false; }
    if (!PEM_write_PrivateKey(fpriv, pkey, NULL, NULL, 0, NULL, NULL)) {
        print_openssl_errors();
        fclose(fpriv);
        EVP_PKEY_free(pkey);
        DSA_free(dsa);
        return false;
    }
    fclose(fpriv);

    // write public key
    FILE* fpub = fopen(pub_out.c_str(), "wb");
    if (!fpub) { std::cerr << "Cannot open " << pub_out << "\n"; EVP_PKEY_free(pkey); DSA_free(dsa); return false; }
    if (!PEM_write_PUBKEY(fpub, pkey)) {
        print_openssl_errors();
        fclose(fpub);
        EVP_PKEY_free(pkey);
        DSA_free(dsa);
        return false;
    }
    fclose(fpub);

    EVP_PKEY_free(pkey);
    DSA_free(dsa);
    return true;
}

static EVP_PKEY* load_private_key(const std::string& path) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return nullptr;
    EVP_PKEY* pkey = PEM_read_PrivateKey(f, NULL, NULL, NULL);
    fclose(f);
    return pkey;
}

static EVP_PKEY* load_public_key(const std::string& path) {
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) return nullptr;
    EVP_PKEY* pkey = PEM_read_PUBKEY(f, NULL, NULL, NULL);
    fclose(f);
    return pkey;
}

bool sign_file_with_dsa(const std::string& priv_pem, const std::string& infile,
                        const std::string& sig_out, bool embed) {
    EVP_PKEY* pkey = load_private_key(priv_pem);
    if (!pkey) { std::cerr << "Failed load private key\n"; return false; }

    // Create context
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) { EVP_PKEY_free(pkey); return false; }

    if (EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
        print_openssl_errors();
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return false;
    }

    // Feed file bytes
    std::ifstream in(infile, std::ios::binary);
    if (!in) { std::cerr << "Cannot open infile\n"; EVP_MD_CTX_free(mdctx); EVP_PKEY_free(pkey); return false; }
    const size_t CHUNK = 8192;
    std::vector<char> buf(CHUNK);
    while (in.good()) {
        in.read(buf.data(), (std::streamsize)CHUNK);
        std::streamsize r = in.gcount();
        if (r > 0) {
            if (EVP_DigestSignUpdate(mdctx, buf.data(), (size_t)r) <= 0) {
                print_openssl_errors();
                EVP_MD_CTX_free(mdctx);
                EVP_PKEY_free(pkey);
                return false;
            }
        }
    }

    size_t siglen = 0;
    if (EVP_DigestSignFinal(mdctx, NULL, &siglen) <= 0) {
        print_openssl_errors();
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return false;
    }
    std::vector<unsigned char> sig(siglen);
    if (EVP_DigestSignFinal(mdctx, sig.data(), &siglen) <= 0) {
        print_openssl_errors();
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        return false;
    }
    sig.resize(siglen);

    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);

    // build container and either write separate or append
    auto container = build_signature_container(sig);
    if (embed) {
        if (!append_file(infile, container)) {
            std::cerr << "Failed to append signature to file\n";
            return false;
        }
    } else {
        if (!write_file_full(sig_out, container)) {
            std::cerr << "Failed to write signature file\n";
            return false;
        }
    }
    return true;
}

bool verify_file_with_dsa(const std::string& pub_pem, const std::string& infile,
                          const std::string& sig_file, bool embedded) {
    EVP_PKEY* pkey = load_public_key(pub_pem);
    if (!pkey) { std::cerr << "Failed load public key\n"; return false; }

    std::vector<unsigned char> sig;
    size_t data_len = 0;

    if (embedded) {
        // Read full file, get footer from EOF
        std::ifstream in(infile, std::ios::binary | std::ios::ate);
        if (!in) { std::cerr << "Cannot open infile\n"; EVP_PKEY_free(pkey); return false; }
        std::streamsize total = in.tellg();
        if (total < (std::streamsize)SigFooter::FOOTER_SIZE) {
            std::cerr << "File too small to contain embedded signature\n";
            EVP_PKEY_free(pkey);
            return false;
        }
        // Read footer
        in.seekg(total - (std::streamsize)SigFooter::FOOTER_SIZE);
        std::vector<unsigned char> footer(SigFooter::FOOTER_SIZE);
        in.read((char*)footer.data(), footer.size());
        uint32_t siglen = 0;
        if (!parse_signature_footer(footer, siglen)) {
            std::cerr << "No signature footer found\n";
            EVP_PKEY_free(pkey);
            return false;
        }
        if (total < (std::streamsize)(siglen + SigFooter::FOOTER_SIZE)) {
            std::cerr << "file shorter than sig+footer\n";
            EVP_PKEY_free(pkey);
            return false;
        }
        // Read signature bytes preceding footer
        in.seekg(total - (std::streamsize)(siglen + SigFooter::FOOTER_SIZE));
        sig.resize(siglen);
        in.read((char*)sig.data(), siglen);
        // Data length is total - siglen - footer
        data_len = (size_t)(total - (std::streamsize)(siglen + SigFooter::FOOTER_SIZE));
        in.seekg(0);
        // prepare to stream verify over first data_len bytes
        EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
        if (!mdctx) { EVP_PKEY_free(pkey); return false; }
        if (EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
            print_openssl_errors(); EVP_MD_CTX_free(mdctx); EVP_PKEY_free(pkey); return false;
        }
        const size_t CHUNK = 8192;
        std::vector<char> buf(CHUNK);
        size_t remaining = data_len;
        while (remaining > 0) {
            size_t toread = std::min(CHUNK, remaining);
            in.read(buf.data(), (std::streamsize)toread);
            std::streamsize r = in.gcount();
            if (r <= 0) break;
            if (EVP_DigestVerifyUpdate(mdctx, buf.data(), (size_t)r) <= 0) {
                print_openssl_errors(); EVP_MD_CTX_free(mdctx); EVP_PKEY_free(pkey); return false;
            }
            remaining -= (size_t)r;
        }
        // verify
        int ok = EVP_DigestVerifyFinal(mdctx, sig.data(), sig.size());
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        if (ok == 1) {
            std::cout << "Signature OK\n";
            return true;
        } else if (ok == 0) {
            std::cout << "Signature INVALID\n";
            return false;
        } else {
            print_openssl_errors();
            return false;
        }
    } else {
        // separate signature file contains [sig][footer]
        auto cont = read_file_full(sig_file);
        if (cont.size() < SigFooter::FOOTER_SIZE) {
            std::cerr << "signature file too small\n"; EVP_PKEY_free(pkey); return false;
        }
        std::vector<unsigned char> footer(cont.end() - SigFooter::FOOTER_SIZE, cont.end());
        uint32_t siglen = 0;
        if (!parse_signature_footer(footer, siglen)) {
            std::cerr << "Invalid signature footer\n"; EVP_PKEY_free(pkey); return false;
        }
        if (cont.size() < siglen + SigFooter::FOOTER_SIZE) {
            std::cerr << "signature file shorter than indicated\n"; EVP_PKEY_free(pkey); return false;
        }
        sig.assign(cont.begin(), cont.begin() + siglen);

        // Now verify file content
        std::ifstream in(infile, std::ios::binary);
        if (!in) { std::cerr << "Cannot open infile\n"; EVP_PKEY_free(pkey); return false; }
        EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
        if (!mdctx) { EVP_PKEY_free(pkey); return false; }
        if (EVP_DigestVerifyInit(mdctx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
            print_openssl_errors(); EVP_MD_CTX_free(mdctx); EVP_PKEY_free(pkey); return false;
        }
        const size_t CHUNK = 8192;
        std::vector<char> buf(CHUNK);
        while (in.good()) {
            in.read(buf.data(), (std::streamsize)CHUNK);
            std::streamsize r = in.gcount();
            if (r > 0) {
                if (EVP_DigestVerifyUpdate(mdctx, buf.data(), (size_t)r) <= 0) {
                    print_openssl_errors(); EVP_MD_CTX_free(mdctx); EVP_PKEY_free(pkey); return false;
                }
            }
        }
        int ok = EVP_DigestVerifyFinal(mdctx, sig.data(), sig.size());
        EVP_MD_CTX_free(mdctx);
        EVP_PKEY_free(pkey);
        if (ok == 1) {
            std::cout << "Signature OK\n";
            return true;
        } else if (ok == 0) {
            std::cout << "Signature INVALID\n";
            return false;
        } else {
            print_openssl_errors();
            return false;
        }
    }
}
