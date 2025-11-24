#include "../include/utils.h"
#include <openssl/sha.h>
#include <fstream>
#include <iostream>

std::vector<unsigned char> sha256_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    const size_t buffer_size = 32768;
    std::vector<unsigned char> buffer(buffer_size);
    while (file) {
        file.read(reinterpret_cast<char*>(buffer.data()), buffer_size);
        size_t bytes = file.gcount();
        if (bytes > 0)
            SHA256_Update(&sha256, buffer.data(), bytes);
    }

    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    SHA256_Final(hash.data(), &sha256);
    return hash;
}

void write_file(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

std::vector<unsigned char> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Cannot read file: " + filename);
    size_t size = file.tellg();
    std::vector<unsigned char> buffer(size);
    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}