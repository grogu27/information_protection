#pragma once
#include <vector>
#include <string>
#include <utility>
#include <cstdint>  // Добавлено для uint64_t

// class GOST341094 {
//     vector<unsigned char> p; // Большое простое число (модуль)
//     vector<unsigned char> q; // Простой делитель (p-1)
//     vector<unsigned char> a; // Образующий элемент (1 < a < p-1)
//     vector<unsigned char> d; // Закрытый ключ (случайное число)
//     vector<unsigned char> c; // Открытый ключ
// };

class GOST341094 {
private:
    std::vector<unsigned char> p, q, a;
    std::vector<unsigned char> d, c;

    // Вспомогательные функции
    std::vector<unsigned char> from_int(uint64_t n);
    std::vector<unsigned char> random_number(const std::vector<unsigned char>& max);
    bool is_zero(const std::vector<unsigned char>& n);
    bool less(const std::vector<unsigned char>& a, const std::vector<unsigned char>& b);
    
public:
    GOST341094();
    void generate_keys();
    std::pair<std::vector<unsigned char>, std::vector<unsigned char>> 
    sign(const std::vector<unsigned char>& message_hash);
    bool verify(const std::vector<unsigned char>& message_hash, 
                const std::pair<std::vector<unsigned char>, std::vector<unsigned char>>& signature);
    void save_signature(const std::pair<std::vector<unsigned char>, std::vector<unsigned char>>& signature, 
                       const std::string& filename);
    std::pair<std::vector<unsigned char>, std::vector<unsigned char>> 
    load_signature(const std::string& filename);
};