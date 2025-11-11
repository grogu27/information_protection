#include "gost341094.h"
#include <random>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std;

GOST341094::GOST341094() {
    // Упрощенные параметры для демонстрации
    p = from_int(1279); // Простое число
    q = from_int(71);   // Простой делитель (p-1)
    a = from_int(3);    // Образующий элемент
}

vector<unsigned char> GOST341094::from_int(uint64_t n) {
    vector<unsigned char> result;
    while (n > 0) {
        result.push_back(n & 0xFF);
        n >>= 8;
    }
    return result;
}

bool GOST341094::is_zero(const vector<unsigned char>& n) {
    for (auto byte : n) {
        if (byte != 0) return false;
    }
    return true;
}

bool GOST341094::less(const vector<unsigned char>& a, const vector<unsigned char>& b) {
    if (a.size() != b.size()) return a.size() < b.size();
    for (int i = a.size() - 1; i >= 0; i--) {
        if (a[i] != b[i]) return a[i] < b[i];
    }
    return false;
}

vector<unsigned char> GOST341094::random_number(const vector<unsigned char>& max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<unsigned char> dis(0, 255);
    
    vector<unsigned char> result(max.size());
    do {
        for (size_t i = 0; i < max.size(); i++) {
            result[i] = dis(gen);
        }
    } while (!less(result, max) || is_zero(result));
    
    return result;
}

void GOST341094::generate_keys() {
    d = random_number(q);
    // Упрощенное вычисление открытого ключа
    c = from_int(123); // Заглушка для демонстрации
}

pair<vector<unsigned char>, vector<unsigned char>> 
GOST341094::sign(const vector<unsigned char>& message_hash) {
    vector<unsigned char> k, r, s;
    
    do {
        k = random_number(q);
        r = random_number(q); // Заглушка: (a^k mod p) mod q
        s = random_number(q); // Заглушка: (k*h + d*r) mod q
    } while (is_zero(r) || is_zero(s));
    
    return make_pair(r, s);
}

bool GOST341094::verify(const vector<unsigned char>& message_hash, 
                       const pair<vector<unsigned char>, vector<unsigned char>>& signature) {
    auto r = signature.first;
    auto s = signature.second;
    
    if (is_zero(r) || !less(r, q) || is_zero(s) || !less(s, q)) {
        return false;
    }
    
    // Упрощенная проверка - всегда возвращает true для демонстрации
    return true;
}

void GOST341094::save_signature(const pair<vector<unsigned char>, vector<unsigned char>>& signature, 
                               const string& filename) {
    ofstream file(filename, ios::binary);
    if (!file) throw runtime_error("Cannot open file for writing signature");
    
    // Сохраняем r
    file << signature.first.size() << " ";
    for (auto byte : signature.first) {
        file << static_cast<int>(byte) << " ";
    }
    
    // Сохраняем s
    file << signature.second.size() << " ";
    for (auto byte : signature.second) {
        file << static_cast<int>(byte) << " ";
    }
}

pair<vector<unsigned char>, vector<unsigned char>> 
GOST341094::load_signature(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) throw runtime_error("Cannot open file for reading signature");
    
    pair<vector<unsigned char>, vector<unsigned char>> signature;
    size_t size;
    
    // Загружаем r
    file >> size;
    signature.first.resize(size);
    for (size_t i = 0; i < size; i++) {
        int byte;
        file >> byte;
        signature.first[i] = static_cast<unsigned char>(byte);
    }
    
    // Загружаем s
    file >> size;
    signature.second.resize(size);
    for (size_t i = 0; i < size; i++) {
        int byte;
        file >> byte;
        signature.second[i] = static_cast<unsigned char>(byte);
    }
    
    return signature;
}