#include "utils.h"
#include <fstream>
#include <cstring>
#include <arpa/inet.h> // htonl
#include <sys/stat.h>

bool file_exists(const std::string &path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

std::vector<unsigned char> read_file_full(const std::string &path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return {};
    in.seekg(0, std::ios::end);
    size_t sz = (size_t)in.tellg();
    in.seekg(0);
    std::vector<unsigned char> buf(sz);
    in.read((char*)buf.data(), sz);
    return buf;
}

bool write_file_full(const std::string &path, const std::vector<unsigned char>& data) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out.write((const char*)data.data(), data.size());
    return true;
}

bool append_file(const std::string &path, const std::vector<unsigned char>& data) {
    std::ofstream out(path, std::ios::binary | std::ios::app);
    if (!out) return false;
    out.write((const char*)data.data(), data.size());
    return true;
}

std::vector<unsigned char> build_signature_container(const std::vector<unsigned char>& sig) {
    SigFooter footer;
    // Prepare footer
    memset(&footer, 0, sizeof(footer));
    const char *magic = "FIPS186SIG\n";
    memcpy(footer.magic, magic, strlen(magic));
    footer.version = 1;
    memcpy(footer.alg, "DSA", 3);
    memcpy(footer.hash, "SHA256", 6);
    uint32_t sig_len_be = htonl((uint32_t)sig.size());
    footer.sig_len_be = sig_len_be;

    std::vector<unsigned char> out;
    out.reserve(sig.size() + SigFooter::FOOTER_SIZE);
    out.insert(out.end(), sig.begin(), sig.end());
    out.insert(out.end(), (unsigned char*)&footer, (unsigned char*)&footer + SigFooter::FOOTER_SIZE);
    return out;
}

bool parse_signature_footer(const std::vector<unsigned char>& footer_bytes, uint32_t &sig_len_out) {
    if (footer_bytes.size() != SigFooter::FOOTER_SIZE) return false;
    SigFooter footer;
    memcpy(&footer, footer_bytes.data(), SigFooter::FOOTER_SIZE);
    // check magic
    const char* magic = "FIPS186SIG\n";
    if (memcmp(footer.magic, magic, strlen(magic)) != 0) return false;
    uint32_t sig_len = ntohl(footer.sig_len_be);
    sig_len_out = sig_len;
    return true;
}
