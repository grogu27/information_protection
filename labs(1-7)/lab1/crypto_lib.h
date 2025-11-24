#ifndef CRYPTO_LIB_H
#define CRYPTO_LIB_H

#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <chrono>

class CryptoLibrary {
private:
    std::mt19937_64 rng; // Генератор случайных чисел
    
public:
    // Конструктор
    CryptoLibrary();
    
    // 1. Быстрое возведение в степень по модулю: y = a^b mod p
    long long mod_pow(long long base, long long exponent, long long modulus);
    
    // Для очень больших чисел (более 64 бит) - используем строковое представление
    std::string mod_pow_large(const std::string& base, const std::string& exponent, const std::string& modulus);
    
    // 2. Тест простоты Ферма
    bool fermat_primality_test(long long n, int k = 10);
    
    // 3. Обобщенный алгоритм Евклида: ax + by = gcd(a,b)
    struct EGCDResult {
        long long gcd;
        long long x;
        long long y;
    };
    EGCDResult extended_gcd(long long a, long long b);
    
    // Вспомогательные функции
    long long generate_random_number(long long min, long long max);
    long long generate_prime(long long min, long long max);
    
    // Ввод с клавиатуры
    std::pair<long long, long long> input_from_keyboard();
    
    // Автоматическая генерация чисел
    std::pair<long long, long long> auto_generate_numbers();
    
    // Автоматическая генерация простых чисел
    std::pair<long long, long long> auto_generate_primes();
};

// Вспомогательные функции для работы с большими числами
std::string multiply_strings(const std::string& num1, const std::string& num2);
std::string add_strings(const std::string& num1, const std::string& num2);
std::string mod_strings(const std::string& dividend, const std::string& divisor);
bool is_greater_or_equal(const std::string& num1, const std::string& num2);

#endif