#pragma once
#include <string>
#include <vector>
#include <cstdint>

bool file_exists(const std::string &path);
std::vector<unsigned char> read_file_full(const std::string &path);
bool write_file_full(const std::string &path, const std::vector<unsigned char>& data);
bool append_file(const std::string &path, const std::vector<unsigned char>& data);

// signature container helpers
struct SigFooter {
    static constexpr size_t FOOTER_SIZE = 32;
    char magic[11]; // "FIPS186SIG\n"
    uint8_t version;
    char alg[8];    // "DSA"
    char hash[8];   // "SHA256"
    uint32_t sig_len_be; // big-endian
};

std::vector<unsigned char> build_signature_container(const std::vector<unsigned char>& sig);
bool parse_signature_footer(const std::vector<unsigned char>& footer_bytes, uint32_t &sig_len_out);
