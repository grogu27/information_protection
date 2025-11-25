#pragma once
#include <string>
#include <vector>
#include <utility>
#include <cstdint>

class FIPS186 {
private:
    std::vector<unsigned char> p, q, g; // параметры
    std::vector<unsigned char> x; // секретный ключ
    std::vector<unsigned char> y; // публичный ключ

    static uint64_t to_uint64(const std::vector<unsigned char>& v);
    static std::vector<unsigned char> from_uint64(uint64_t n);

    static uint64_t mod_mul(uint64_t a, uint64_t b, uint64_t mod);
    static uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod);
    static uint64_t mod_inv(uint64_t a, uint64_t mod);

    std::vector<unsigned char> random_number(const std::vector<unsigned char>& max);
    bool is_zero(const std::vector<unsigned char>& n);
    bool less(const std::vector<unsigned char>& a, const std::vector<unsigned char>& b);

public:
    FIPS186();

    void generate_keys(); // генерирует x и y
    std::pair<std::vector<unsigned char>, std::vector<unsigned char>>
      sign(const std::vector<unsigned char>& message_hash); // возвращает (r, s)
    bool verify(const std::vector<unsigned char>& message_hash,
                const std::pair<std::vector<unsigned char>, std::vector<unsigned char>>& signature);

    void save_signature(const std::pair<std::vector<unsigned char>, std::vector<unsigned char>>& signature,
                        const std::string& filename);
    std::pair<std::vector<unsigned char>, std::vector<unsigned char>>
      load_signature(const std::string& filename);
};

// Утилитная функция, похожая на gost_sign.h интерфейс
void fips_sign_and_verify_file(const std::string& filename);
